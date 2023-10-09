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

#include "trpc/tracing/jaeger/jaeger_client_filter.h"

#include "trpc/common/config/trpc_config.h"
#include "trpc/tracing/tracing_factory.h"
#include "trpc/util/log/logging.h"

#include "trpc/tracing/jaeger/jaeger_text_map_carrier.h"

namespace trpc {

int JaegerClientFilter::Init() {
  auto tracing = TracingFactory::GetInstance()->Get(trpc::jaeger::kJaegerTracingName);
  if (!tracing) {
    TRPC_FMT_ERROR("JaegerClientFilter init failed: plugin {} has not been registered",
                   trpc::jaeger::kJaegerTracingName);
    return -1;
  }
  tracer_factory_ = trpc::dynamic_pointer_cast<JaegerTracing>(tracing);
  if (!tracer_factory_) {
    TRPC_FMT_ERROR("the type of {} instance is invalid.", trpc::jaeger::kJaegerTracingName);
    return -1;
  }
  return 0;
}

std::vector<FilterPoint> JaegerClientFilter::GetFilterPoint() {
  std::vector<FilterPoint> points = {FilterPoint::CLIENT_PRE_RPC_INVOKE, FilterPoint::CLIENT_POST_RPC_INVOKE};
  return points;
}

void JaegerClientFilter::operator()(FilterStatus& status, FilterPoint point, const ClientContextPtr& context) {
  status = FilterStatus::CONTINUE;

  if (!tracer_factory_) {
    return;
  }

  // ensures that client_span is not empty
  ClientTracingSpan* client_span = trpc::jaeger::detail::GetClientSpan(context, tracer_factory_->GetPluginID());

  if (point == FilterPoint::CLIENT_PRE_RPC_INVOKE) {
    client_span->span = std::move(NewSpan(context, client_span->parent_span));
  } else if (point == FilterPoint::CLIENT_POST_RPC_INVOKE) {
    FinishSpan(client_span->span, context);
  }
}

std::shared_ptr<::opentracing::Tracer> JaegerClientFilter::GetTracer(const ClientContextPtr& context) {
  std::shared_ptr<::opentracing::Tracer> tracer;

  if (context->GetCallerName().empty()) {
    return ::opentracing::Tracer::Global();
  }

  std::string err_msg;
  auto jaeger_tracer = tracer_factory_->MakeTracer(context->GetCallerName().c_str(), err_msg);
  if (jaeger_tracer) {
    tracer = *jaeger_tracer;
  } else {
    tracer = ::opentracing::Tracer::Global();
  }

  return tracer;
}

trpc::jaeger::JaegerTracingSpanPtr JaegerClientFilter::NewSpan(const ClientContextPtr& context,
                                                               const std::any& any_parent_span) {
  const ::opentracing::SpanContext* parent_span_ctx = nullptr;

  if (trpc::jaeger::detail::CheckSpanTypeValid(any_parent_span)) {
    const auto& parent_span = std::any_cast<const trpc::jaeger::JaegerTracingSpanPtr&>(any_parent_span);
    parent_span_ctx = &(parent_span->context());
  }

  std::shared_ptr<::opentracing::Tracer> tracer = GetTracer(context);

  auto span = tracer->StartSpan(
      context->GetFuncName(),
      {::opentracing::ChildOf(parent_span_ctx),
       ::opentracing::SetTag{::opentracing::ext::span_kind, ::opentracing::ext::span_kind_rpc_client}});
  // injects information into the context
  if (auto* req_trans_info = context->GetMutablePbReqTransInfo(); req_trans_info) {
    JaegerTextMapWriter carrier(req_trans_info);
    if (!tracer->Inject(span->context(), carrier)) {
      TRPC_FMT_ERROR("caller:{} callee:{} request_id:{} codec_name:{} JaegerClientFilter Inject fail",
                     context->GetCallerName(), context->GetCalleeName(), context->GetRequestId(),
                     context->GetCodecName());
    }
  }

  return trpc::jaeger::JaegerTracingSpanPtr(std::move(span));
}

void JaegerClientFilter::FinishSpan(const std::any& any_span, const ClientContextPtr& context) {
  if (!trpc::jaeger::detail::CheckSpanTypeValid(any_span)) {
    return;
  }
  const auto& span = std::any_cast<const trpc::jaeger::JaegerTracingSpanPtr&>(any_span);

  // sets status
  trpc::jaeger::detail::SetStatus(context, span);

  span->SetTag(trpc::jaeger::kTraceExtResponseSize, context->GetResponseLength());
  span->SetTag(::opentracing::ext::peer_service, context->GetCalleeName());
  span->SetTag(::opentracing::ext::peer_hostname, context->GetTargetMetadata(naming::kNodeContainerName));
  if (context->GetIsIpv6()) {
    span->SetTag(::opentracing::ext::peer_host_ipv6, context->GetIp());
  } else {
    span->SetTag(::opentracing::ext::peer_host_ipv4, context->GetIp());
  }
  span->SetTag(::opentracing::ext::peer_port, context->GetPort());
  span->SetTag(trpc::jaeger::kTraceExtNamespace, TrpcConfig::GetInstance()->GetGlobalConfig().env_namespace);
  span->SetTag(trpc::jaeger::kTraceExtEnvName, TrpcConfig::GetInstance()->GetGlobalConfig().env_name);

  span->Finish();
}

}  // namespace trpc
