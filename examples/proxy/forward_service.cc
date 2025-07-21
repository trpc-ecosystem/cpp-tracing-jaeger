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

#include "examples/proxy/forward_service.h"

#include "trpc/client/client_context.h"
#include "trpc/client/make_client_context.h"
#include "trpc/client/trpc_client.h"
#include "trpc/log/trpc_log.h"

#include "trpc/tracing/jaeger/jaeger_tracing_api.h"

namespace examples::forward {

ForwardServiceImpl::ForwardServiceImpl() {
  greeter_proxy_ =
      ::trpc::GetTrpcClient()->GetProxy<::trpc::test::helloworld::GreeterServiceProxy>("trpc.test.helloworld.Greeter");
}

::trpc::Status ForwardServiceImpl::Route(::trpc::ServerContextPtr context,
                                         const ::trpc::test::helloworld::HelloRequest* request,
                                         ::trpc::test::helloworld::HelloReply* reply) {
  TRPC_FMT_INFO("Forward request:{}, req id:{}", request->msg(), context->GetRequestId());

  // gets the current span and performs some operation provided by the OpenTracing API
  ::trpc::jaeger::JaegerTracingSpanPtr span = ::trpc::jaeger::GetSpan(context);
  if (span) {
    TRPC_FMT_INFO("Set tag to jaeger span:{}, {}", "request_msg", request->msg());
    span->SetTag("request_msg", request->msg());
  }

  // uses the ServerContext to construct ClientContext so that the entire call chain can be linked together.
  auto client_context = ::trpc::MakeClientContext(context, greeter_proxy_);

  ::trpc::test::helloworld::HelloRequest route_request;
  route_request.set_msg(request->msg());
  ::trpc::test::helloworld::HelloReply route_reply;
  // block current fiber, not block current fiber worker thread
  ::trpc::Status status = greeter_proxy_->SayHello(client_context, route_request, &route_reply);

  TRPC_FMT_INFO("Forward status:{}, route_reply:{}", status.ToString(), route_reply.msg());

  reply->set_msg(route_reply.msg());

  return status;
}

}  // namespace examples::forward
