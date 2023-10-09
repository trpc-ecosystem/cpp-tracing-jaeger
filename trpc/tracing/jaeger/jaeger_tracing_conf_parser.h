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

#include "yaml-cpp/yaml.h"

#include "trpc/tracing/jaeger/jaeger_tracing_conf.h"

namespace YAML {

template <>
struct convert<trpc::JaegerBaggageRestrictConfig> {
  static YAML::Node encode(const trpc::JaegerBaggageRestrictConfig& config) {
    Node node;

    node["denyBaggageOnInitializationFailure"] = config.deny_baggage_on_initialization_failure;
    node["hostPort"] = config.host_port;
    node["refreshInterval"] = config.refresh_interval;

    return node;
  }

  static bool decode(const YAML::Node& node, trpc::JaegerBaggageRestrictConfig& config) {
    if (node["denyBaggageOnInitializationFailure"]) {
      config.deny_baggage_on_initialization_failure = node["denyBaggageOnInitializationFailure"].as<bool>();
    }

    if (node["hostPort"]) {
      config.host_port = node["hostPort"].as<std::string>();
    }

    if (node["refreshInterval"]) {
      config.refresh_interval = node["refreshInterval"].as<int>();
    }

    return true;
  }
};

template <>
struct convert<trpc::JaegerHeadersConfig> {
  static YAML::Node encode(const trpc::JaegerHeadersConfig& config) {
    Node node;

    node["jaegerDebugHeader"] = config.jaeger_debug_header;
    node["jaegerBaggageHeader"] = config.jaeger_baggage_header;
    node["TraceContextHeaderName"] = config.trace_context_header_name;
    node["traceBaggageHeaderPrefix"] = config.trace_baggage_header_prefix;

    return node;
  }

  static bool decode(const YAML::Node& node, trpc::JaegerHeadersConfig& config) {
    if (node["jaegerDebugHeader"]) {
      config.jaeger_debug_header = node["jaegerDebugHeader"].as<std::string>();
    }

    if (node["jaegerBaggageHeader"]) {
      config.jaeger_baggage_header = node["jaegerBaggageHeader"].as<std::string>();
    }

    if (node["TraceContextHeaderName"]) {
      config.trace_context_header_name = node["TraceContextHeaderName"].as<std::string>();
    }

    if (node["traceBaggageHeaderPrefix"]) {
      config.trace_baggage_header_prefix = node["traceBaggageHeaderPrefix"].as<std::string>();
    }

    return true;
  }
};

template <>
struct convert<trpc::JaegerReporterConfig> {
  static YAML::Node encode(const trpc::JaegerReporterConfig& config) {
    Node node;

    node["queueSize"] = config.queue_size;
    node["bufferFlushInterval"] = config.buffer_flush_interval;
    node["logSpans"] = config.log_spans;
    node["localAgentHostPort"] = config.local_agent_host_port;
    node["endpoint"] = config.endpoint;

    return node;
  }

  static bool decode(const YAML::Node& node, trpc::JaegerReporterConfig& config) {
    if (node["queueSize"]) {
      config.queue_size = node["queueSize"].as<int>();
    }

    if (node["bufferFlushInterval"]) {
      config.buffer_flush_interval = node["bufferFlushInterval"].as<int>();
    }

    if (node["logSpans"]) {
      config.log_spans = node["logSpans"].as<bool>();
    }

    if (node["localAgentHostPort"]) {
      config.local_agent_host_port = node["localAgentHostPort"].as<std::string>();
    }

    if (node["endpoint"]) {
      config.endpoint = node["endpoint"].as<std::string>();
    }

    return true;
  }
};

template <>
struct convert<trpc::JaegerSamplerConfig> {
  static YAML::Node encode(const trpc::JaegerSamplerConfig& config) {
    Node node;

    node["type"] = config.type;
    node["param"] = config.param;
    node["samplingServerURL"] = config.sampling_server_url;
    node["maxOperations"] = config.max_operations;
    node["samplingRefreshInterval"] = config.sampling_refresh_interval;

    return node;
  }

  static bool decode(const YAML::Node& node, trpc::JaegerSamplerConfig& config) {
    if (node["type"]) {
      config.type = node["type"].as<std::string>();
    }

    if (node["param"]) {
      config.param = node["param"].as<double>();
    }

    if (node["samplingServerURL"]) {
      config.sampling_server_url = node["samplingServerURL"].as<std::string>();
    }

    if (node["maxOperations"]) {
      config.max_operations = node["maxOperations"].as<int>();
    }

    if (node["samplingRefreshInterval"]) {
      config.sampling_refresh_interval = node["samplingRefreshInterval"].as<int>();
    }

    return true;
  }
};

template <>
struct convert<trpc::JaegerConfig> {
  static YAML::Node encode(const trpc::JaegerConfig& config) {
    YAML::Node node;

    node["service_name"] = config.service_name;
    node["disabled"] = config.disabled;
    node["traceid_128bit"] = config.traceid_128bit;
    node["propagation_format"] = config.propagation_format;
    node["enable_logger"] = config.enable_logger;
    node["sampler"] = config.sampler;
    node["reporter"] = config.reporter;
    node["headers"] = config.headers;
    node["baggage_restrictions"] = config.baggage_restrictions;

    return node;
  }

  static bool decode(const YAML::Node& node, trpc::JaegerConfig& config) {
    if (node["service_name"]) {
      config.service_name = node["service_name"].as<std::string>();
    }

    if (node["disabled"]) {
      config.disabled = node["disabled"].as<bool>();
    }

    if (node["traceid_128bit"]) {
      config.traceid_128bit = node["traceid_128bit"].as<bool>();
    }

    if (node["propagation_format"]) {
      config.propagation_format = node["propagation_format"].as<std::string>();
    }

    if (node["enable_logger"]) {
      config.enable_logger = node["enable_logger"].as<bool>();
    }

    if (node["sampler"]) {
      config.sampler = node["sampler"].as<trpc::JaegerSamplerConfig>();
    }

    if (node["reporter"]) {
      config.reporter = node["reporter"].as<trpc::JaegerReporterConfig>();
    }

    if (node["headers"]) {
      config.headers = node["headers"].as<trpc::JaegerHeadersConfig>();
    }

    if (node["baggage_restrictions"]) {
      config.baggage_restrictions = node["baggage_restrictions"].as<trpc::JaegerBaggageRestrictConfig>();
    }

    return true;
  }
};

}  // namespace YAML
