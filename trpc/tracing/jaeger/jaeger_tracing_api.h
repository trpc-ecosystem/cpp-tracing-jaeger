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

#include "trpc/server/server_context.h"

#include "trpc/tracing/jaeger/jaeger_common.h"

/// @brief Jaeger interfaces for user programing
namespace trpc::jaeger {

/// @brief Initializes the Jaeger tracing plugin and filters.
bool Init();

/// @brief Definition of an invalid index returned by the GetFilterDataIndex interface.
constexpr uint32_t kInvalidFilterDataIndex = 0;

/// @brief Gets the filter data index of the Jaeger plugin, which can be used to get or set tracing data.
/// @return Return the filter index. Note that kInvalidFilterDataIndex will be returned when the Jaeger plugin is not
///         registered correctly.
uint32_t GetFilterDataIndex();

/// @brief Gets the span.
/// @param context server context
/// @return Return the span saved in the context. Note that nullptr will be returned when there is no valid span in the
///         context.
JaegerTracingSpanPtr GetSpan(const ServerContextPtr& context);

}  // namespace trpc::jaeger
