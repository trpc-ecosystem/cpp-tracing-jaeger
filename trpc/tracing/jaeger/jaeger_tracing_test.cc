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

#include "trpc/tracing/jaeger/jaeger_tracing.h"

#include "gtest/gtest.h"
#include "trpc/common/config/trpc_config.h"

namespace trpc::testing {

class JaegerTracingTest : public ::testing::Test {
 protected:
  static void SetUpTestCase() { jaeger_tracing_ = MakeRefCounted<JaegerTracing>(); }
  static void TearDownTestCase() {}

 protected:
  static JaegerTracingPtr jaeger_tracing_;
};
JaegerTracingPtr JaegerTracingTest::jaeger_tracing_;

TEST_F(JaegerTracingTest, Init) {
  ASSERT_EQ(trpc::jaeger::kJaegerTracingName, jaeger_tracing_->Name());

  // 1. init failed because jaeger plugin is not configured
  ASSERT_NE(0, jaeger_tracing_->Init());

  // 2. init success because jaeger plugin had configured
  TrpcConfig::GetInstance()->Init("./trpc/tracing/jaeger/testing/jaeger_tracing.yaml");
  ASSERT_EQ(0, jaeger_tracing_->Init());
}

TEST_F(JaegerTracingTest, MakeTracer) {
  // test for using invalid service_name
  std::string err_msg;
  auto invalid_tracer = jaeger_tracing_->MakeTracer(nullptr, err_msg);
  ASSERT_FALSE(invalid_tracer);

  // test for getting tracer normally
  std::string service_a = "service_a";
  auto ser_a_tracer_1 = jaeger_tracing_->MakeTracer(service_a.c_str(), err_msg);
  ASSERT_TRUE(ser_a_tracer_1);
  auto ser_a_tracer_2 = jaeger_tracing_->MakeTracer(service_a.c_str(), err_msg);
  ASSERT_TRUE(ser_a_tracer_2);
  // identical service_names will use the same tracer
  ASSERT_EQ(ser_a_tracer_1, ser_a_tracer_2);

  std::string service_b = "service_b";
  auto ser_b_tracer = jaeger_tracing_->MakeTracer(service_b.c_str(), err_msg);
  ASSERT_TRUE(ser_b_tracer);
  // different service_names will use different tracers
  ASSERT_NE(ser_a_tracer_1, ser_b_tracer);
}

}  // namespace trpc::testing
