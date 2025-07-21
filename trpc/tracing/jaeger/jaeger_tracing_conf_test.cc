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

#include "trpc/tracing/jaeger/jaeger_tracing_conf.h"
#include "trpc/tracing/jaeger/jaeger_tracing_conf_parser.h"

#include "gtest/gtest.h"
#include "yaml-cpp/yaml.h"

namespace trpc::testing {

TEST(JaegerConfigTest, ConfCodec) {
  trpc::JaegerConfig jaeger_config;
  jaeger_config.service_name = "my_service";
  jaeger_config.disabled = false;
  jaeger_config.traceid_128bit = false;
  jaeger_config.propagation_format = "jaeger";
  jaeger_config.enable_logger = false;
  jaeger_config.sampler.type = "const";
  jaeger_config.sampler.param = 2;
  jaeger_config.sampler.sampling_server_url = "";
  jaeger_config.sampler.max_operations = 1;
  jaeger_config.sampler.sampling_refresh_interval = 2;
  jaeger_config.reporter.queue_size = 10;
  jaeger_config.reporter.buffer_flush_interval = 3;
  jaeger_config.reporter.log_spans = true;
  jaeger_config.reporter.local_agent_host_port = "1234";
  jaeger_config.reporter.endpoint = "5678";
  jaeger_config.headers.jaeger_debug_header = "debug_header";
  jaeger_config.headers.jaeger_baggage_header = "baggage_header";
  jaeger_config.headers.trace_context_header_name = "ctx_header";
  jaeger_config.headers.trace_baggage_header_prefix = "baggage_header_prefix";
  jaeger_config.baggage_restrictions.deny_baggage_on_initialization_failure = false;
  jaeger_config.baggage_restrictions.host_port = "1234";
  jaeger_config.baggage_restrictions.refresh_interval = 0;

  jaeger_config.Display();

  auto node = YAML::convert<trpc::JaegerConfig>::encode(jaeger_config);

  JaegerConfig decode_jaeger_config;
  ASSERT_TRUE(YAML::convert<trpc::JaegerConfig>::decode(node, decode_jaeger_config));

  ASSERT_EQ(jaeger_config.service_name, decode_jaeger_config.service_name);
  ASSERT_EQ(jaeger_config.disabled, decode_jaeger_config.disabled);
  ASSERT_EQ(jaeger_config.traceid_128bit, decode_jaeger_config.traceid_128bit);
  ASSERT_EQ(jaeger_config.propagation_format, decode_jaeger_config.propagation_format);
  ASSERT_EQ(jaeger_config.enable_logger, decode_jaeger_config.enable_logger);
  ASSERT_EQ(jaeger_config.sampler.type, decode_jaeger_config.sampler.type);
  ASSERT_EQ(jaeger_config.sampler.param, decode_jaeger_config.sampler.param);
  ASSERT_EQ(jaeger_config.sampler.sampling_server_url, decode_jaeger_config.sampler.sampling_server_url);
  ASSERT_EQ(jaeger_config.sampler.max_operations, decode_jaeger_config.sampler.max_operations);
  ASSERT_EQ(jaeger_config.sampler.sampling_refresh_interval, decode_jaeger_config.sampler.sampling_refresh_interval);
  ASSERT_EQ(jaeger_config.reporter.queue_size, decode_jaeger_config.reporter.queue_size);
  ASSERT_EQ(jaeger_config.reporter.buffer_flush_interval, decode_jaeger_config.reporter.buffer_flush_interval);
  ASSERT_EQ(jaeger_config.reporter.log_spans, decode_jaeger_config.reporter.log_spans);
  ASSERT_EQ(jaeger_config.reporter.local_agent_host_port, decode_jaeger_config.reporter.local_agent_host_port);
  ASSERT_EQ(jaeger_config.reporter.endpoint, decode_jaeger_config.reporter.endpoint);
  ASSERT_EQ(jaeger_config.headers.jaeger_debug_header, decode_jaeger_config.headers.jaeger_debug_header);
  ASSERT_EQ(jaeger_config.headers.jaeger_baggage_header, decode_jaeger_config.headers.jaeger_baggage_header);
  ASSERT_EQ(jaeger_config.headers.trace_context_header_name, decode_jaeger_config.headers.trace_context_header_name);
  ASSERT_EQ(jaeger_config.headers.trace_baggage_header_prefix,
            decode_jaeger_config.headers.trace_baggage_header_prefix);
  ASSERT_EQ(jaeger_config.baggage_restrictions.deny_baggage_on_initialization_failure,
            decode_jaeger_config.baggage_restrictions.deny_baggage_on_initialization_failure);
  ASSERT_EQ(jaeger_config.baggage_restrictions.host_port, decode_jaeger_config.baggage_restrictions.host_port);
  ASSERT_EQ(jaeger_config.baggage_restrictions.refresh_interval,
            decode_jaeger_config.baggage_restrictions.refresh_interval);
}

}  // namespace trpc::testing
