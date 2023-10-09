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

#include "trpc/tracing/jaeger/jaeger_common.h"

namespace trpc::jaeger {

namespace detail {

bool CheckSpanTypeValid(const std::any& span) {
  if (span.type() != typeid(JaegerTracingSpanPtr) || std::any_cast<const JaegerTracingSpanPtr&>(span) == nullptr) {
    return false;
  }
  return true;
}

ClientTracingSpan* GetClientSpan(const ClientContextPtr& context, uint32_t filter_data_index) {
  ClientTracingSpan* client_span = context->GetFilterData<ClientTracingSpan>(filter_data_index);
  if (!client_span) {
    context->SetFilterData(filter_data_index, ClientTracingSpan());
    client_span = context->GetFilterData<ClientTracingSpan>(filter_data_index);
  }
  return client_span;
}

}  // namespace detail

}  // namespace trpc::jaeger
