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

#pragma once

#include "trpc/client/client_context.h"
#include "trpc/filter/filter.h"
#include "trpc/tracing/jaeger/jaeger_common.h"
#include "trpc/tracing/jaeger/jaeger_tracing.h"

namespace trpc {

class JaegerClientFilter : public MessageClientFilter {
 public:
  int Init() override;

  std::string Name() override { return trpc::jaeger::kJaegerTracingName; }

  std::vector<FilterPoint> GetFilterPoint() override;

  void operator()(FilterStatus& status, FilterPoint point, const ClientContextPtr& context) override;

 private:
  // Gets a tracer from tracer_factory_
  std::shared_ptr<::opentracing::Tracer> GetTracer(const ClientContextPtr& context);

  // Creates a new span
  trpc::jaeger::JaegerTracingSpanPtr NewSpan(const ClientContextPtr& context, const std::any& any_parent_span);

  // Finishes the span
  void FinishSpan(const std::any& any_span, const ClientContextPtr& context);

 private:
  JaegerTracingPtr tracer_factory_ = nullptr;
};

}  // namespace trpc
