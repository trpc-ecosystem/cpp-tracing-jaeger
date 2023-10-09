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

#include "trpc/tracing/jaeger/jaeger_tracing_api.h"

#include "trpc/common/trpc_plugin.h"
#include "trpc/tracing/jaeger/jaeger_client_filter.h"
#include "trpc/tracing/jaeger/jaeger_server_filter.h"
#include "trpc/tracing/jaeger/jaeger_tracing.h"
#include "trpc/tracing/tracing_factory.h"
#include "trpc/tracing/tracing_filter_index.h"

namespace trpc::jaeger {

bool Init() {
  TrpcPlugin::GetInstance()->RegisterTracing(MakeRefCounted<JaegerTracing>());
  TrpcPlugin::GetInstance()->RegisterServerFilter(std::make_shared<JaegerServerFilter>());
  TrpcPlugin::GetInstance()->RegisterClientFilter(std::make_shared<JaegerClientFilter>());
  return true;
}

namespace {

TracingPtr GetPlugin() {
  TracingPtr tracing = TracingFactory::GetInstance()->Get(trpc::jaeger::kJaegerTracingName);
  if (!tracing) {
    TRPC_FMT_DEBUG("get tracing plugin {} failed.", trpc::jaeger::kJaegerTracingName);
    return nullptr;
  }
  return tracing;
}

}  // namespace

uint32_t GetFilterDataIndex() {
  auto tracing = GetPlugin();
  if (tracing) {
    return tracing->GetPluginID();
  }
  return kInvalidFilterDataIndex;
}

JaegerTracingSpanPtr GetSpan(const ServerContextPtr& context) {
  uint32_t filter_index = GetFilterDataIndex();
  if (filter_index != kInvalidFilterDataIndex) {
    auto* server_tracing_span = context->GetFilterData<ServerTracingSpan>(filter_index);
    if (server_tracing_span) {
      auto* tracing_span = std::any_cast<JaegerTracingSpanPtr>(&server_tracing_span->span);
      if (tracing_span) {
        return *tracing_span;
      }
    }
  }
  return nullptr;
}

}  // namespace trpc::jaeger
