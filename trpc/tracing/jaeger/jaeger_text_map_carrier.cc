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

#include "trpc/util/log/logging.h"

namespace trpc {

::opentracing::expected<void> JaegerTextMapWriter::Set(::opentracing::string_view key,
                                                       ::opentracing::string_view value) const {
  if (text_map_) {
    (*text_map_)[key] = value;
  } else {
    TRPC_LOG_TRACE("set fail! text_map is null");
  }
  return {};
}

::opentracing::expected<::opentracing::string_view> JaegerTextMapReader::LookupKey(
    ::opentracing::string_view key) const {
  auto iter = text_map_.find(key);
  if (iter != text_map_.end()) {
    return ::opentracing::string_view{iter->second};
  } else {
    return ::opentracing::make_unexpected(::opentracing::key_not_found_error);
  }
}

::opentracing::expected<void> JaegerTextMapReader::ForeachKey(
    std::function<::opentracing::expected<void>(::opentracing::string_view key, ::opentracing::string_view value)> f)
    const {
  for (const auto& key_value : text_map_) {
    auto result = f(key_value.first, key_value.second);
    if (!result) return result;
  }
  return {};
}

}  // namespace trpc
