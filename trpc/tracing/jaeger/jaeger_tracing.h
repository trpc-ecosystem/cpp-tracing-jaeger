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

#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "jaegertracing/Config.h"
#include "jaegertracing/Tracer.h"
#include "opentracing/tracer_factory.h"
#include "trpc/tracing/tracing.h"

#include "trpc/tracing/jaeger/jaeger_common.h"
#include "trpc/tracing/jaeger/jaeger_tracing_conf.h"

namespace trpc {

/// @brief Implementation of Jaeger tracing plugin. Jaeger (https://www.jaegertracing.io/) is an open-source distributed
///        tracing system developed by Uber, which follows the OpenTracing distributed tracing specification
///        (https://opentracing.io/specification/).
class JaegerTracing : public Tracing, public ::opentracing::TracerFactory {
 public:
  std::string Name() const override { return trpc::jaeger::kJaegerTracingName; }

  int Init() noexcept override;

  /// @brief Gets a tracer with the requested service name
  /// @param service_name used to identify the tracer
  /// @param error_message used to record error information when getting fails
  /// @return Returns the corresponding tracer on success. Otherwise, an error is returned.
  ::opentracing::expected<std::shared_ptr<::opentracing::Tracer>> MakeTracer(const char* service_name,
                                                                             std::string& error_message) const
      noexcept override;

 private:
  // Used to store the tracers corresponding to different service names.
  mutable std::unordered_map<std::string, std::shared_ptr<::opentracing::Tracer>> tracers_;
  mutable std::shared_mutex mutex_;

  // The configuration of jaeger
  JaegerConfig config_;
  ::jaegertracing::Config jaeger_conf_;
};

using JaegerTracingPtr = RefPtr<JaegerTracing>;

}  // namespace trpc
