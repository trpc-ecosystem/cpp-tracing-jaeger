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

#include "examples/server/greeter_service.h"

#include <string>

#include "trpc/log/trpc_log.h"

#include "trpc/tracing/jaeger/jaeger_tracing_api.h"

namespace test {
namespace helloworld {

::trpc::Status GreeterServiceImpl::SayHello(::trpc::ServerContextPtr context,
                                            const ::trpc::test::helloworld::HelloRequest* request,
                                            ::trpc::test::helloworld::HelloReply* reply) {
  // gets the current span and performs some operation provided by the OpenTracing API
  ::trpc::jaeger::JaegerTracingSpanPtr span = ::trpc::jaeger::GetSpan(context);
  if (span) {
    TRPC_FMT_INFO("Set tag to jaeger span:{}, {}", "request_msg", request->msg());
    span->SetTag("request_msg", request->msg());
  }

  std::string response = "Hello, " + request->msg();
  reply->set_msg(response);

  return ::trpc::kSuccStatus;
}

}  // namespace helloworld
}  // namespace test
