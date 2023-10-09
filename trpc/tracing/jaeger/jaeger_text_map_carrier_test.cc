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

#include "trpc/tracing/jaeger/jaeger_text_map_carrier.h"

#include "gtest/gtest.h"

namespace trpc::testing {

TEST(JaegerTextMapCarrierTestTest, Set) {
  google::protobuf::Map<std::string, std::string> text_map;
  trpc::JaegerTextMapWriter carrier(&text_map);
  carrier.Set("testkey", "testvalue");
  ASSERT_NE(text_map.end(), text_map.find("testkey"));
  ASSERT_EQ("testvalue", text_map["testkey"]);

  ASSERT_EQ(text_map.end(), text_map.find("invalidkey"));

  // Even if the text_map parameter is nullptr, it should not result in a core dump.
  trpc::JaegerTextMapWriter null_carrier(nullptr);
  null_carrier.Set("testkey", "testvalue");
}

TEST(JaegerTextMapReaderTest, LookupKey) {
  google::protobuf::Map<std::string, std::string> text_map;
  text_map["testkey"] = "testvalue";
  trpc::JaegerTextMapReader carrier(text_map);
  ASSERT_TRUE(carrier.LookupKey("testkey"));
  ASSERT_EQ("testvalue", *(carrier.LookupKey("testkey")));

  ASSERT_FALSE(carrier.LookupKey("invalidkey"));
}

TEST(JaegerTextMapReaderTest, ForeachKey) {
  google::protobuf::Map<std::string, std::string> text_map;
  text_map["testkey"] = "testvalue";
  trpc::JaegerTextMapReader carrier(text_map);
  std::string for_res;
  ASSERT_FALSE(carrier.ForeachKey(
      [&for_res](::opentracing::string_view key, ::opentracing::string_view value) -> ::opentracing::expected<void> {
        if ("testkey" == key) {
          EXPECT_EQ("testvalue", value);
          return ::opentracing::make_unexpected(std::error_code{});
        }
        return {};
      }));

  ASSERT_TRUE(carrier.ForeachKey(
      [&](::opentracing::string_view key, ::opentracing::string_view value) -> ::opentracing::expected<void> {
        if ("invalidkey" == key) {
          return ::opentracing::make_unexpected(std::error_code{});
        }
        return {};
      }));
}

}  // namespace trpc::testing
