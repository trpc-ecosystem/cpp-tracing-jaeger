//
//
// Tencent is pleased to support the open source community by making tRPC available.
//
// Copyright (C) 2023 Tencent.
// All rights reserved.
//
// If you have downloaded a copy of the tRPC source code from Tencent,
// please note that tRPC source code is licensed under the  Apache 2.0 License,
// A copy of the Apache 2.0 License is included in this file.
//
//

#include "trpc/tracing/jaeger/jaeger_tracing_api.h"

#include "gtest/gtest.h"
#include "opentracing/ext/tags.h"
#include "trpc/client/testing/service_proxy_testing.h"
#include "trpc/codec/trpc/testing/trpc_protocol_testing.h"
#include "trpc/common/config/trpc_config.h"
#include "trpc/proto/testing/helloworld.pb.h"
#include "trpc/server/rpc/rpc_service_impl.h"
#include "trpc/server/testing/server_context_testing.h"
#include "trpc/tracing/tracing_factory.h"
#include "trpc/tracing/tracing_filter_index.h"

#include "trpc/tracing/jaeger/jaeger_tracing.h"

namespace trpc::testing {

class JaegerTracingAPITest : public ::testing::Test {
 protected:
  static void SetUpTestCase() {
    int ret = TrpcConfig::GetInstance()->Init("./trpc/tracing/jaeger/testing/jaeger_tracing.yaml");
    ASSERT_EQ(0, ret);
    RegisterPlugins();

    trpc_service_ = std::make_shared<RpcServiceImpl>();

    // registers JaegerTracing plugin
    tracing_ = MakeRefCounted<JaegerTracing>();
    ASSERT_EQ(0, tracing_->Init());
    TracingFactory::GetInstance()->Register(tracing_);
  }

  static void TearDownTestCase() { UnregisterPlugins(); }

  static ServerContextPtr GetTestServerContext() {
    DummyTrpcProtocol req_data;
    trpc::test::helloworld::HelloRequest hello_req;
    NoncontiguousBuffer req_bin_data;
    PackTrpcRequest(req_data, static_cast<void*>(&hello_req), req_bin_data);
    ServerContextPtr context = MakeTestServerContext("trpc", trpc_service_.get(), std::move(req_bin_data));

    // sets span
    std::string err_msg;
    auto jaeger_tracer = tracing_->MakeTracer(context->GetCallerName().c_str(), err_msg);
    std::shared_ptr<::opentracing::Tracer> tracer = *jaeger_tracer;
    // creates a parent span and save its trace id and span id
    auto parent_span = tracer->StartSpan(
        context->GetFuncName(),
        {::opentracing::ChildOf(nullptr),
         ::opentracing::SetTag{::opentracing::ext::span_kind, ::opentracing::ext::span_kind_rpc_client}});
    ServerTracingSpan server_span;
    server_span.span = std::move(trpc::jaeger::JaegerTracingSpanPtr(std::move(parent_span)));
    context->SetFilterData(tracing_->GetPluginID(), std::move(server_span));

    return context;
  }

 protected:
  static JaegerTracingPtr tracing_;
  static std::shared_ptr<RpcServiceImpl> trpc_service_;
};

JaegerTracingPtr JaegerTracingAPITest::tracing_;
std::shared_ptr<RpcServiceImpl> JaegerTracingAPITest::trpc_service_;

TEST_F(JaegerTracingAPITest, GetFilterDataIndex) {
  uint32_t index = trpc::jaeger::GetFilterDataIndex();
  ASSERT_NE(trpc::jaeger::kInvalidFilterDataIndex, index);
  ASSERT_EQ(tracing_->GetPluginID(), index);
}

TEST_F(JaegerTracingAPITest, GetSpan) {
  ServerContextPtr context = GetTestServerContext();
  ASSERT_NE(nullptr, trpc::jaeger::GetSpan(context));
}

}  // namespace trpc::testing
