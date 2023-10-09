//
//
// Tencent is pleased to support the open source community by making tRPC available.
//
// Copyright (C) 2023 THL A29 Limited, a Tencent company.
// All rights reserved.
//
// If you have downloaded a copy of the tRPC source code from Tencent,
// please note that tRPC source code is licensed under the  Apache 2.0 License,
// A copy of the Apache 2.0 License is included in this file.
//
//

#include "trpc/tracing/jaeger/jaeger_server_filter.h"

#include <sstream>

#include "gtest/gtest.h"
#include "jaegertracing/Constants.h"
#include "trpc/codec/codec_manager.h"
#include "trpc/codec/trpc/testing/trpc_protocol_testing.h"
#include "trpc/common/config/trpc_config.h"
#include "trpc/proto/testing/helloworld.pb.h"
#include "trpc/serialization/trpc_serialization.h"
#include "trpc/server/rpc/rpc_service_impl.h"
#include "trpc/server/testing/server_context_testing.h"
#include "trpc/tracing/tracing_factory.h"
#include "trpc/tracing/tracing_filter_index.h"

namespace trpc::testing {

class JaegerServerFilterTest : public ::testing::Test {
 protected:
  static void SetUpTestCase() {
    int ret = TrpcConfig::GetInstance()->Init("./trpc/tracing/jaeger/testing/jaeger_tracing.yaml");
    ASSERT_EQ(0, ret);

    codec::Init();
    serialization::Init();

    tracing_ = MakeRefCounted<JaegerTracing>();
    ASSERT_EQ(0, tracing_->Init());
    TracingFactory::GetInstance()->Register(tracing_);
    server_filter_ = std::make_shared<JaegerServerFilter>();
    ASSERT_EQ(0, server_filter_->Init());
  }

  static void TearDownTestCase() {
    codec::Destroy();
    serialization::Destroy();
  }

 protected:
  static MessageServerFilterPtr server_filter_;
  static TracingPtr tracing_;
};
MessageServerFilterPtr JaegerServerFilterTest::server_filter_;
TracingPtr JaegerServerFilterTest::tracing_;

TEST_F(JaegerServerFilterTest, Init) {
  ASSERT_EQ(trpc::jaeger::kJaegerTracingName, server_filter_->Name());
  std::vector<FilterPoint> points = server_filter_->GetFilterPoint();
  ASSERT_EQ(2, points.size());
  ASSERT_TRUE(std::find(points.begin(), points.end(), FilterPoint::SERVER_POST_RECV_MSG) != points.end());
  ASSERT_TRUE(std::find(points.begin(), points.end(), FilterPoint::SERVER_PRE_SEND_MSG) != points.end());
}

TEST_F(JaegerServerFilterTest, TraceWithoutUpstreamInfo) {
  DummyTrpcProtocol req_data;
  trpc::test::helloworld::HelloRequest hello_req;
  NoncontiguousBuffer req_bin_data;
  ASSERT_TRUE(PackTrpcRequest(req_data, static_cast<void*>(&hello_req), req_bin_data));
  std::shared_ptr<RpcServiceImpl> test_rpc_server_impl = std::make_shared<RpcServiceImpl>();
  // There is no tracing information in the context's transinfo.
  ServerContextPtr context = MakeTestServerContext("trpc", test_rpc_server_impl.get(), std::move(req_bin_data));

  // test for SERVER_POST_RECV_MSG filter point
  FilterStatus status;
  server_filter_->operator()(status, FilterPoint::SERVER_POST_RECV_MSG, context);
  ASSERT_EQ(FilterStatus::CONTINUE, status);
  ServerTracingSpan* ptr = context->GetFilterData<ServerTracingSpan>(tracing_->GetPluginID());
  ASSERT_NE(nullptr, ptr);
  ASSERT_EQ(typeid(trpc::jaeger::JaegerTracingSpanPtr), ptr->span.type());

  // test for SERVER_PRE_SEND_MSG filter point
  trpc::Status frame_status;
  frame_status.SetFrameworkRetCode(101);
  context->SetStatus(frame_status);
  server_filter_->operator()(status, FilterPoint::SERVER_PRE_SEND_MSG, context);
  ASSERT_EQ(FilterStatus::CONTINUE, status);
}

TEST_F(JaegerServerFilterTest, TraceWithUpstreamInfo) {
  DummyTrpcProtocol req_data;
  trpc::test::helloworld::HelloRequest hello_req;
  NoncontiguousBuffer req_bin_data;
  ASSERT_TRUE(PackTrpcRequest(req_data, static_cast<void*>(&hello_req), req_bin_data));
  std::shared_ptr<RpcServiceImpl> test_rpc_server_impl = std::make_shared<RpcServiceImpl>();
  // There has tracing information in the context's transinfo.
  ServerContextPtr context = MakeTestServerContext("trpc", test_rpc_server_impl.get(), std::move(req_bin_data));
  std::string parent_trace_id = "1000000000000000";
  std::string parent_span_id = "2000000000000000";
  std::string trace_info_str = parent_trace_id + ":" + parent_span_id + ":0000000000000000:1";
  context->AddReqTransInfo(::jaegertracing::kTraceContextHeaderName, trace_info_str);

  // test for SERVER_POST_RECV_MSG filter point
  FilterStatus status;
  server_filter_->operator()(status, FilterPoint::SERVER_POST_RECV_MSG, context);
  ASSERT_EQ(FilterStatus::CONTINUE, status);
  ServerTracingSpan* ptr = context->GetFilterData<ServerTracingSpan>(tracing_->GetPluginID());
  ASSERT_NE(nullptr, ptr);
  ASSERT_EQ(typeid(trpc::jaeger::JaegerTracingSpanPtr), ptr->span.type());

  // the trace id of the current span should be equal to the parent_trace_id,
  // the span id of the current should be a newly generated ID
  // the parent id of the current should be equal to the parent_span_id.
  const auto& span = std::any_cast<const trpc::jaeger::JaegerTracingSpanPtr&>(ptr->span);
  auto jaeger_span = dynamic_cast<const jaegertracing::Span*>(span.get());
  ASSERT_NE(nullptr, jaeger_span);
  std::ostringstream oss;
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

  // test for SERVER_PRE_SEND_MSG filter point
  trpc::Status frame_status;
  frame_status.SetFrameworkRetCode(101);
  context->SetStatus(frame_status);
  server_filter_->operator()(status, FilterPoint::SERVER_PRE_SEND_MSG, context);
  ASSERT_EQ(FilterStatus::CONTINUE, status);
}

}  // namespace trpc::testing
