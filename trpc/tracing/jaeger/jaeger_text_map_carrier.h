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

#include "google/protobuf/map.h"
#include "opentracing/propagation.h"

namespace trpc {

/// @brief A read-write class that assists in setting tracing information to transinfo.
class JaegerTextMapWriter : public ::opentracing::TextMapWriter {
 public:
  explicit JaegerTextMapWriter(google::protobuf::Map<std::string, std::string>* text_map) : text_map_(text_map) {}

  ::opentracing::expected<void> Set(::opentracing::string_view key, ::opentracing::string_view value) const override;

 private:
  google::protobuf::Map<std::string, std::string>* text_map_;
};

/// @brief A read-only class that assists in extracting tracing information from transinfo.
class JaegerTextMapReader : public ::opentracing::TextMapReader {
 public:
  explicit JaegerTextMapReader(const google::protobuf::Map<std::string, std::string>& text_map) : text_map_(text_map) {}

  ::opentracing::expected<::opentracing::string_view> LookupKey(::opentracing::string_view key) const override;

  ::opentracing::expected<void> ForeachKey(
      std::function<::opentracing::expected<void>(::opentracing::string_view key, ::opentracing::string_view value)> f)
      const override;

 private:
  const google::protobuf::Map<std::string, std::string>& text_map_;
};

}  // namespace trpc
