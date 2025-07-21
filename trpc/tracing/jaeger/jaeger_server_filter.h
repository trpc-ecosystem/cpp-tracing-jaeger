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

#pragma once

#include "trpc/filter/filter.h"
#include "trpc/server/server_context.h"
#include "trpc/tracing/jaeger/jaeger_common.h"
#include "trpc/tracing/jaeger/jaeger_tracing.h"

namespace trpc {

class JaegerServerFilter : public trpc::MessageServerFilter {
 public:
  int Init() override;

  std::string Name() override { return trpc::jaeger::kJaegerTracingName; }

  std::vector<FilterPoint> GetFilterPoint() override;

  void operator()(FilterStatus& status, FilterPoint point, const ServerContextPtr& context) override;

 protected:
  // Gets a tracer from tracer_factory_
  std::shared_ptr<::opentracing::Tracer> GetTracer(const ServerContextPtr& context);

  // Creates a new span
  trpc::jaeger::JaegerTracingSpanPtr NewSpan(const ServerContextPtr& context);

  // Finishes the span
  void FinishSpan(const std::any& any_span, const ServerContextPtr& context);

 protected:
  JaegerTracingPtr tracer_factory_ = nullptr;
};

}  // namespace trpc
