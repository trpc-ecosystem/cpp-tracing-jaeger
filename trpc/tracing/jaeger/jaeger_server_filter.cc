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

#include "trpc/tracing/jaeger/jaeger_server_filter.h"

#include "trpc/common/config/trpc_config.h"
#include "trpc/tracing/tracing_factory.h"
#include "trpc/tracing/tracing_filter_index.h"
#include "trpc/util/log/logging.h"

#include "trpc/tracing/jaeger/jaeger_text_map_carrier.h"

namespace trpc {

int JaegerServerFilter::Init() {
  auto tracing = TracingFactory::GetInstance()->Get(trpc::jaeger::kJaegerTracingName);
  if (!tracing) {
    TRPC_FMT_ERROR("JaegerServerFilter init failed: plugin {} has not been registered",
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

std::vector<FilterPoint> JaegerServerFilter::GetFilterPoint() {
  std::vector<FilterPoint> points = {FilterPoint::SERVER_POST_RECV_MSG, FilterPoint::SERVER_PRE_SEND_MSG};
  return points;
}

void JaegerServerFilter::operator()(FilterStatus& status, FilterPoint point, const ServerContextPtr& context) {
  status = FilterStatus::CONTINUE;

  if (!tracer_factory_) {
    return;
  }

  if (point == FilterPoint::SERVER_POST_RECV_MSG) {
    ServerTracingSpan svr_span;
    svr_span.span = std::move(NewSpan(context));
    context->SetFilterData<ServerTracingSpan>(tracer_factory_->GetPluginID(), std::move(svr_span));
  } else if (point == FilterPoint::SERVER_PRE_SEND_MSG) {
    ServerTracingSpan* ptr = context->GetFilterData<ServerTracingSpan>(tracer_factory_->GetPluginID());
    if (ptr) {
      FinishSpan(ptr->span, context);
    }
  }
}

std::shared_ptr<::opentracing::Tracer> JaegerServerFilter::GetTracer(const ServerContextPtr& context) {
  if (context->GetCalleeName().empty()) {
    return ::opentracing::Tracer::Global();
  }

  std::shared_ptr<::opentracing::Tracer> tracer;
  std::string err_msg("");
  auto jaeger_tracer = tracer_factory_->MakeTracer(context->GetCalleeName().c_str(), err_msg);
  if (jaeger_tracer) {
    tracer = *jaeger_tracer;
  } else {
    tracer = ::opentracing::Tracer::Global();
  }

  return tracer;
}

trpc::jaeger::JaegerTracingSpanPtr JaegerServerFilter::NewSpan(const ServerContextPtr& context) {
  auto& req_trans_info = context->GetPbReqTransInfo();
  JaegerTextMapReader carrier(req_trans_info);
  std::shared_ptr<::opentracing::Tracer> tracer = GetTracer(context);
  auto exp_span_ctx = tracer->Extract(carrier);
  const ::opentracing::SpanContext* parent_span_ctx = nullptr;
  if (exp_span_ctx) {
    parent_span_ctx = exp_span_ctx->get();
  }

  auto span = tracer->StartSpan(
      context->GetFuncName(),
      {
          ::opentracing::ChildOf(parent_span_ctx),
          ::opentracing::SetTag{::opentracing::ext::span_kind, ::opentracing::ext::span_kind_rpc_server},
      });
  return trpc::jaeger::JaegerTracingSpanPtr(std::move(span));
}

void JaegerServerFilter::FinishSpan(const std::any& any_span, const ServerContextPtr& context) {
  if (!trpc::jaeger::detail::CheckSpanTypeValid(any_span)) {
    return;
  }
  const auto& span = std::any_cast<const trpc::jaeger::JaegerTracingSpanPtr&>(any_span);

  // sets status
  trpc::jaeger::detail::SetStatus(context, span);

  // sets env info
  span->SetTag(trpc::jaeger::kTraceExtNamespace, TrpcConfig::GetInstance()->GetGlobalConfig().env_namespace);
  span->SetTag(trpc::jaeger::kTraceExtEnvName, TrpcConfig::GetInstance()->GetGlobalConfig().env_name);
  // sets request packet size
  span->SetTag(trpc::jaeger::kTraceExtRequestSize, context->GetRequestLength());

  span->Finish();
}

}  // namespace trpc
