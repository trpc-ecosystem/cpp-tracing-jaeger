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

#include "trpc/tracing/jaeger/jaeger_client_filter.h"

#include <sstream>

#include "gtest/gtest.h"
#include "opentracing/ext/tags.h"
#include "trpc/codec/trpc/trpc_client_codec.h"
#include "trpc/common/config/trpc_config.h"
#include "trpc/tracing/tracing_factory.h"
#include "trpc/tracing/tracing_filter_index.h"

namespace trpc::testing {

class JaegerClientFilterTest : public ::testing::Test {
 protected:
  static void SetUpTestCase() {
    int ret = TrpcConfig::GetInstance()->Init("./trpc/tracing/jaeger/testing/jaeger_tracing.yaml");
    ASSERT_EQ(0, ret);
    tracing_ = MakeRefCounted<JaegerTracing>();
    ASSERT_EQ(0, tracing_->Init());
    TracingFactory::GetInstance()->Register(tracing_);
    client_filter_ = std::make_shared<JaegerClientFilter>();
    ASSERT_EQ(0, client_filter_->Init());
  }
  static void TearDownTestCase() {}

 protected:
  static MessageClientFilterPtr client_filter_;
  static JaegerTracingPtr tracing_;
};
MessageClientFilterPtr JaegerClientFilterTest::client_filter_;
JaegerTracingPtr JaegerClientFilterTest::tracing_;

TEST_F(JaegerClientFilterTest, Init) {
  ASSERT_EQ(trpc::jaeger::kJaegerTracingName, client_filter_->Name());
  std::vector<FilterPoint> points = client_filter_->GetFilterPoint();
  ASSERT_EQ(2, points.size());
  ASSERT_TRUE(std::find(points.begin(), points.end(), FilterPoint::CLIENT_PRE_RPC_INVOKE) != points.end());
  ASSERT_TRUE(std::find(points.begin(), points.end(), FilterPoint::CLIENT_POST_RPC_INVOKE) != points.end());
}

TEST_F(JaegerClientFilterTest, TraceWithoutParentSpan) {
  auto trpc_codec = std::make_shared<trpc::TrpcClientCodec>();
  trpc::ClientContextPtr context = trpc::MakeRefCounted<trpc::ClientContext>(trpc_codec);
  context->SetCallerName("trpc.test.helloworld.client");
  // There is no parent span in ClientTracingSpan.
  context->SetFilterData(tracing_->GetPluginID(), ClientTracingSpan());

  // test for CLIENT_PRE_RPC_INVOKE filter point
  FilterStatus status;
  client_filter_->operator()(status, FilterPoint::CLIENT_PRE_RPC_INVOKE, context);
  ASSERT_EQ(FilterStatus::CONTINUE, status);
  ClientTracingSpan* ptr = context->GetFilterData<ClientTracingSpan>(tracing_->GetPluginID());
  ASSERT_NE(nullptr, ptr);
  ASSERT_EQ(typeid(trpc::jaeger::JaegerTracingSpanPtr), ptr->span.type());

  // test for CLIENT_POST_RPC_INVOKE filter point
  trpc::Status frame_status;
  frame_status.SetFrameworkRetCode(101);
  frame_status.SetErrorMessage("timeout");
  context->SetStatus(frame_status);
  client_filter_->operator()(status, FilterPoint::CLIENT_POST_RPC_INVOKE, context);
  ASSERT_EQ(FilterStatus::CONTINUE, status);
}

TEST_F(JaegerClientFilterTest, TraceWithParentSpan) {
  auto trpc_codec = std::make_shared<trpc::TrpcClientCodec>();
  trpc::ClientContextPtr context = trpc::MakeRefCounted<trpc::ClientContext>(trpc_codec);
  context->SetCallerName("trpc.test.helloworld.client");
  // There has parent span in ClientTracingSpan.
  std::string err_msg;
  auto jaeger_tracer = tracing_->MakeTracer(context->GetCallerName().c_str(), err_msg);
  ASSERT_TRUE(jaeger_tracer);
  std::shared_ptr<::opentracing::Tracer> tracer = *jaeger_tracer;
  // creates a parent span and save its trace id and span id
  auto parent_span = tracer->StartSpan(
      context->GetFuncName(),
      {::opentracing::ChildOf(nullptr),
       ::opentracing::SetTag{::opentracing::ext::span_kind, ::opentracing::ext::span_kind_rpc_client}});
  auto jaeger_parent_span = dynamic_cast<const jaegertracing::Span*>(parent_span.get());
  ASSERT_NE(nullptr, jaeger_parent_span);
  std::ostringstream oss;
  oss << jaeger_parent_span->context().traceID();
  std::string parent_trace_id = oss.str();
  oss.str("");
  oss << jaeger_parent_span->context().spanID();
  std::string parent_span_id = oss.str();
  oss.str("");
  ClientTracingSpan client_span;
  client_span.parent_span = std::move(trpc::jaeger::JaegerTracingSpanPtr(std::move(parent_span)));
  context->SetFilterData(tracing_->GetPluginID(), std::move(client_span));

  // test for CLIENT_PRE_RPC_INVOKE filter point
  FilterStatus status;
  client_filter_->operator()(status, FilterPoint::CLIENT_PRE_RPC_INVOKE, context);
  ASSERT_EQ(FilterStatus::CONTINUE, status);
  ClientTracingSpan* ptr = context->GetFilterData<ClientTracingSpan>(tracing_->GetPluginID());
  ASSERT_NE(nullptr, ptr);
  ASSERT_EQ(typeid(trpc::jaeger::JaegerTracingSpanPtr), ptr->span.type());

  // the trace id of the current span should be equal to the parent_trace_id,
  // the span id of the current should be a newly generated ID
  // the parent id of the current should be equal to the parent_span_id.
  const auto& span = std::any_cast<const trpc::jaeger::JaegerTracingSpanPtr&>(ptr->span);
  auto jaeger_span = dynamic_cast<const jaegertracing::Span*>(span.get());
  ASSERT_NE(nullptr, jaeger_span);
  oss << jaeger_span->context().traceID();
  std::string trace_id = oss.str();
  oss.str("");
  ASSERT_EQ(parent_trace_id, trace_id);
  oss << jaeger_span->context().spanID();
  std::string span_id = oss.str();
  oss.str("");
  ASSERT_NE(parent_span_id, span_id);
  oss << jaeger_span->context().parentID();
  std::string parent_id = oss.str();
  oss.str("");
  ASSERT_EQ(parent_span_id, parent_id);

  // test for CLIENT_POST_RPC_INVOKE filter point
  trpc::Status frame_status;
  frame_status.SetFrameworkRetCode(101);
  frame_status.SetErrorMessage("timeout");
  context->SetStatus(frame_status);
  client_filter_->operator()(status, FilterPoint::CLIENT_POST_RPC_INVOKE, context);
  ASSERT_EQ(FilterStatus::CONTINUE, status);
}

}  // namespace trpc::testing
