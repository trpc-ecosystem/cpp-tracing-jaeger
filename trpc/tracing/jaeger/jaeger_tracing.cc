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

#include "trpc/tracing/jaeger/jaeger_tracing.h"

#include "trpc/common/config/trpc_config.h"
#include "trpc/util/log/logging.h"

#include "trpc/tracing/jaeger/jaeger_tracing_conf_parser.h"

namespace trpc {

int JaegerTracing::Init() noexcept {
  bool ret = TrpcConfig::GetInstance()->GetPluginConfig("tracing", "jaeger", config_);
  if (!ret) {
    TRPC_LOG_ERROR("get jaeger tracing config failed.");
    return -1;
  }

  jaeger_conf_ = ::jaegertracing::Config::parse(YAML::convert<trpc::JaegerConfig>::encode(config_));
  return 0;
}

::opentracing::expected<std::shared_ptr<::opentracing::Tracer>> JaegerTracing::MakeTracer(
    const char* service_name, std::string& error_message) const noexcept {
  if (nullptr == service_name) {
    error_message = "input service_name is nullptr";
    return ::opentracing::make_unexpected(::opentracing::invalid_configuration_error);
  }

  {
    // acquires a read lock and check if there is already a tracer for the corresponding service name
    std::shared_lock<std::shared_mutex> locker(mutex_);

    auto itr = tracers_.find(service_name);
    if (itr != tracers_.end()) {
      return itr->second;
    }
  }

  // acquires a write lock
  std::unique_lock<std::shared_mutex> locker(mutex_);

  auto itr = tracers_.find(service_name);
  if (itr != tracers_.end()) {  // double check
    return itr->second;
  }

  // creates a new tracer according to the config
  std::shared_ptr<jaegertracing::logging::Logger> logger =
      config_.enable_logger ? jaegertracing::logging::consoleLogger() : jaegertracing::logging::nullLogger();
  auto tracer = jaegertracing::Tracer::make(service_name, jaeger_conf_, logger);

  tracers_[service_name] = tracer;
  return tracer;
}

}  // namespace trpc
