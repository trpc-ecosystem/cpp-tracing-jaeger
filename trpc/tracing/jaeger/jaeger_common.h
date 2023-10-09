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

#include <any>
#include <memory>

#include "opentracing/ext/tags.h"
#include "opentracing/span.h"
#include "trpc/client/client_context.h"
#include "trpc/tracing/tracing_filter_index.h"

namespace trpc::jaeger {

/// @brief Jaeger tracing name
constexpr char kJaegerTracingName[] = "jaeger";

/// @brief The jaeger tracing data that saves in the context for transmission
using JaegerTracingSpanPtr = std::shared_ptr<::opentracing::Span>;

/// @brief The key definition that reported by the filter
constexpr char kTraceExtRet[] = "_trace_ext.ret";
constexpr char kTraceExtDyeingKey[] = "_trace_ext.dyeing_key";
constexpr char kTraceExtNamespace[] = "_trace_ext.namespace";
constexpr char kTraceExtEnvName[] = "_trace_ext.envname";
/// @brief It represents the size of the request packet in bytes, with a data type of uint64.
constexpr char kTraceExtRequestSize[] = "_trace_ext.request.size";
/// @brief It represents the size of the response packet in bytes, with a data type of uint64.
constexpr char kTraceExtResponseSize[] = "_trace_ext.response.size";
/// @brief It's used to indicate whether an error has occurred in the link, where 0 indicates that there is no error and
///        1 indicates that an error has occurred.
constexpr char kTraceExtTraceErrorFlag[] = "_trace_ext.trace.error_flag";

namespace detail {

/// @brief Checks if the type of span is JaegerTracingSpanPtr.
bool CheckSpanTypeValid(const std::any& span);

/// @brief Sets the status info into span
template <typename Context>
void SetStatus(const Context& context, const JaegerTracingSpanPtr& span) {
  // sets error code
  if (context->GetStatus().GetFrameworkRetCode() != 0) {
    span->SetTag(kTraceExtRet, context->GetStatus().GetFrameworkRetCode());
  } else {
    span->SetTag(kTraceExtRet, context->GetStatus().GetFuncRetCode());
  }

  // sets error flag
  if (!context->GetStatus().OK()) {
    span->SetTag(::opentracing::ext::error, true);
    span->Log({{"event", "error"}, {"message", context->GetStatus().ErrorMessage()}});
  }
}

/// @brief Gets the ClientTracingSpan from context
ClientTracingSpan* GetClientSpan(const ClientContextPtr& context, uint32_t filter_data_index);

}  // namespace detail

}  // namespace trpc::jaeger
