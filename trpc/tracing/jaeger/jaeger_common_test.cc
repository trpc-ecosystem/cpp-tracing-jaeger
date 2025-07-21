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

#include "trpc/tracing/jaeger/jaeger_common.h"

#include "gtest/gtest.h"
#include "opentracing/noop.h"
#include "trpc/codec/trpc/trpc_client_codec.h"

namespace trpc::testing {

TEST(JaegerCommonTest, CheckSpanTypeValid) {
  auto noop_tracer = ::opentracing::MakeNoopTracer();
  auto noop_span = noop_tracer->StartSpan("test");
  std::any jaeger_span = trpc::jaeger::JaegerTracingSpanPtr(std::move(noop_span));
  ASSERT_TRUE(trpc::jaeger::detail::CheckSpanTypeValid(jaeger_span));

  std::any other_span = 1;
  ASSERT_FALSE(trpc::jaeger::detail::CheckSpanTypeValid(other_span));
}

TEST(JaegerCommonTest, GetClientSpan) {
  auto trpc_codec = std::make_shared<trpc::TrpcClientCodec>();
  trpc::ClientContextPtr context = trpc::MakeRefCounted<trpc::ClientContext>(trpc_codec);
  ASSERT_NE(nullptr, trpc::jaeger::detail::GetClientSpan(context, 0));
}

}  // namespace trpc::testing
