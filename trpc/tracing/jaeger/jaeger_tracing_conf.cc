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

#include "trpc/tracing/jaeger/jaeger_tracing_conf.h"
#include "trpc/util/log/logging.h"

namespace trpc {

void JaegerSamplerConfig::Display() const {
  TRPC_LOG_DEBUG("---------------Sampler---------------");

  TRPC_FMT_DEBUG("type: {}", type);
  TRPC_FMT_DEBUG("param: {}", param);
  TRPC_FMT_DEBUG("sampling_server_url: {}", sampling_server_url);
  TRPC_FMT_DEBUG("max_operations: {}", max_operations);
  TRPC_FMT_DEBUG("sampling_refresh_interval: {}", sampling_refresh_interval);

  TRPC_LOG_DEBUG("");
}

void JaegerReporterConfig::Display() const {
  TRPC_LOG_DEBUG("---------------Reporter---------------");

  TRPC_FMT_DEBUG("queue_size: {}", queue_size);
  TRPC_FMT_DEBUG("buffer_flush_interval: {}", buffer_flush_interval);
  TRPC_FMT_DEBUG("log_spans: {}", log_spans ? "true" : "false");
  TRPC_FMT_DEBUG("local_agent_host_port: {}", local_agent_host_port);
  TRPC_FMT_DEBUG("endpoint: {}", endpoint);

  TRPC_LOG_DEBUG("");
}

void JaegerHeadersConfig::Display() const {
  TRPC_LOG_DEBUG("---------------Headers---------------");

  TRPC_FMT_DEBUG("jaeger_debug_header: {}", jaeger_debug_header);
  TRPC_FMT_DEBUG("jaeger_baggage_header: {}", jaeger_baggage_header);
  TRPC_FMT_DEBUG("trace_context_header_name: {}", trace_context_header_name);
  TRPC_FMT_DEBUG("trace_baggage_header_prefix: {}", trace_baggage_header_prefix);

  TRPC_LOG_DEBUG("");
}

void JaegerBaggageRestrictConfig::Display() const {
  TRPC_LOG_DEBUG("---------------Baggage---------------");

  TRPC_FMT_DEBUG("deny_baggage_on_initialization_failure: {}",
                 deny_baggage_on_initialization_failure ? "true" : "false");
  TRPC_FMT_DEBUG("host_port: {}", host_port);
  TRPC_FMT_DEBUG("refresh_interval: {}", refresh_interval);

  TRPC_LOG_DEBUG("");
}

void JaegerConfig::Display() const {
  TRPC_LOG_DEBUG("=============Jaeger Config starts=============");

  TRPC_FMT_DEBUG("service_name: {}", service_name);
  TRPC_FMT_DEBUG("disabled: {}", disabled ? "true" : "false");
  TRPC_FMT_DEBUG("traceid_128bit: {}", traceid_128bit ? "true" : "false");
  TRPC_FMT_DEBUG("propagation_format: {}", propagation_format);
  TRPC_FMT_DEBUG("enable_logger: {}", enable_logger ? "true" : "false");
  sampler.Display();
  reporter.Display();
  headers.Display();
  baggage_restrictions.Display();

  TRPC_LOG_DEBUG("=============Jaeger Config ends=============");
}

}  // namespace trpc
