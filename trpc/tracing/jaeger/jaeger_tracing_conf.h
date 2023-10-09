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

#include <string>

#include "yaml-cpp/yaml.h"

namespace trpc {

struct JaegerSamplerConfig {
  std::string type = "const";
  double param = 1;
  std::string sampling_server_url;
  int max_operations = 0;
  int sampling_refresh_interval = 0;

  void Display() const;
};

struct JaegerReporterConfig {
  int queue_size = 0;
  int buffer_flush_interval = 0;
  bool log_spans = true;
  std::string local_agent_host_port;
  std::string endpoint;

  void Display() const;
};

struct JaegerHeadersConfig {
  std::string jaeger_debug_header;
  std::string jaeger_baggage_header;
  std::string trace_context_header_name;
  std::string trace_baggage_header_prefix;

  void Display() const;
};

struct JaegerBaggageRestrictConfig {
  bool deny_baggage_on_initialization_failure = false;
  std::string host_port;
  int refresh_interval = 0;

  void Display() const;
};

/// @brief Configuration of Jaeger tracing plugin.
/// @note Its parameter field supports all configurable fields in jaeger-client-cpp 0.9.0.
///       "enable_logger" is a newly added configuration in trpc-cpp, which is used to control whether to print span
///       information. It is set to false by default.
struct JaegerConfig {
  std::string service_name;
  bool disabled = false;
  bool traceid_128bit = false;
  std::string propagation_format = "jaeger";
  bool enable_logger = false;
  JaegerSamplerConfig sampler;
  JaegerReporterConfig reporter;
  JaegerHeadersConfig headers;
  JaegerBaggageRestrictConfig baggage_restrictions;

  void Display() const;
};

}  // namespace trpc
