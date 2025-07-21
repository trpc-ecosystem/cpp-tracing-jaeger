[中文](./README.zh_CN.md)

[![LICENSE](https://img.shields.io/badge/license-Apache--2.0-green.svg)](https://github.com/trpc-ecosystem/cpp-tracing-jaeger/blob/main/LICENSE)
[![Releases](https://img.shields.io/github/release/trpc-ecosystem/cpp-tracing-jaeger.svg?style=flat-square)](https://github.com/trpc-ecosystem/cpp-tracing-jaeger/releases)
[![Build Status](https://github.com/trpc-ecosystem/cpp-tracing-jaeger/actions/workflows/ci.yml/badge.svg)](https://github.com/trpc-ecosystem/cpp-tracing-jaeger/actions/workflows/ci.yml)
[![Coverage](https://codecov.io/gh/trpc-ecosystem/cpp-tracing-jaeger/branch/main/graph/badge.svg)](https://app.codecov.io/gh/trpc-ecosystem/cpp-tracing-jaeger/tree/main)

# Overview

[Jaeger](https://www.jaegertracing.io/) is an open-source, distributed tracing system that follows the [OpenTracing](https://opentracing.io/) standard. To facilitate user integration with the Jaeger system, we provide a Jaeger Tracing plugin that enables convenient collection and reporting of Jaeger tracing data.

# Usage

For detailed examples, please refer to the [Jaeger examples](./examples/).

## Import dependencies

### Bazel

1. Import repository

    In the project's `WORKSPACE` file, import the `cpp-tracing-jaeger` repository and its dependencies:
    ```
    load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

    git_repository(
        name = "trpc_cpp",
        remote = "https://github.com/trpc-group/trpc-cpp.git",
        branch = "main",
    )

    load("@trpc_cpp//trpc:workspace.bzl", "trpc_workspace")
    trpc_workspace()

    git_repository(
        name = "cpp-tracing-jaeger",
        remote = "https://github.com/trpc-ecosystem/cpp-tracing-jaeger.git",
        branch = "main",
    )

    load("@cpp-tracing-jaeger//trpc:workspace.bzl", "tracing_jaeger_workspace")
    tracing_jaeger_workspace()
    ```

    The [jaeger-client-cpp](https://github.com/jaegertracing/jaeger-client-cpp) has many dependencies on third-party libraries, and it can be cumbersome to compile it entirely from source using Bazel. To simplify the compilation process, we introduce jaeger-client-cpp (v0.9.0) and its necessary dependency library, thrift (v0.12.0-p0 version), through a local library approach. Before compiling, users need to install jaeger-client-cpp and thrift into the /usr/local directory.

2. Import plugin

    Import the "`trpc/tracing/jaeger:jaeger_tracing_api`" dependency in the targets that require Jaeger. For example:
    ```
    cc_binary(
        name = "helloworld_server",
        srcs = ["helloworld_server.cc"],
        deps = [
            "@cpp-tracing-jaeger//trpc/tracing/jaeger:jaeger_tracing_api",
            ...
        ],
    )
    ```

### CMake

Not supported yet.

## Plugin configuration

To use the Jaeger plugin, you must add the plugin configuration to the framework configuration file:
```yaml
plugins:
  tracing:
    jaeger:
      disabled: false
      enable_logger: false
      reporter:
        localAgentHostPort: 127.0.0.1:6831
      sampler:
        type: const
        param: 1
```

The plugin supports all yaml configuration parameters of jaeger-client-cpp v0.9.0. Here, we will only explain the configuration options specific to the plugin and some key configuration options of jaeger-client-cpp. For other configuration parameters, please refer to the [configuration of jaeger-client-cpp](https://github.com/jaegertracing/jaeger-client-cpp/blob/v0.9.0/src/jaegertracing/Config.h).

| Configuration options | Type | Value | Description |
| ------ | ------ | ------ | ------ |
| enable_logger | bool | Default value is false. | Controls whether to print Span information. |
| disabled | bool | Default value is false. | Controls whether to disable tracing data collection and reporting. |
| sampler | Mapping | The default value for "type" is "const" and the default value for "param" is 1, indicating full sampling. | Sets [sampling strategy](https://www.jaegertracing.io/docs/1.21/sampling/). |
| reporter:localAgentHostPort | string | Default value is "127.0.0.1:6831". | The reporting address of tracing data |

## Enable ClientFilter

Just add the Jaeger filter in the client configuration of the framework:
```yaml
client:
  filter:
    - jaeger
```

The `operationName` of the Span created by the ClientFilter is the name of the callee interface, which obtained from ClientContext::GetFuncName. Additionally, the filter will automatically sets the following data into the Span:

| Key | Type | Value |
| ------ | ------ | ------ |
| span.kind | Tag | "client" |
| peer.service | Tag | The name of the callee service. |
| peer.hostname | Tag | The name of the callee container. |
| peer.ipv4/peer.ipv6 | Tag | The IP address of the callee service. |
| peer.port | Tag | The port of the callee service. |
| _trace_ext.response.size | Tag | The size of the response packet in bytes. |
| _trace_ext.namespace | Tag | The namespace. |
| _trace_ext.envname | Tag | The env. |
| _trace_ext.ret | Tag | The error code of the invocation. |
| error | Tag | true (It is only set when error occurs during the invocation) |
| event | Log | "error" (It is only set when error occurs during the invocation) |
| message | Log | The error message. (It is only set when error occurs during the invocation) |

## Enable ServerFilter

Just add the Jaeger filter in the server configuration of the framework:
```yaml
server:
  filter:
    - jaeger
```

The `operationName` of the Span created by the ServerFilter is the name of the callee interface, which obtained from ServerContext::GetFuncName. Additionally, the filter will automatically sets the following data into the Span:

| Key | Type | Value |
| ------ | ------ | ------ |
| span.kind | Tag | "server" |
| _trace_ext.request.size | Tag | The size of the request packet in bytes. |
| _trace_ext.namespace | Tag | The namespace. |
| _trace_ext.envname | Tag | The env. |
| _trace_ext.ret | Tag | The error code of the invocation. |
| error | Tag | true (It is only set when error occurs during the invocation) |
| event | Log | "error" (It is only set when error occurs during the invocation) |
| message | Log | The error message. (It is only set when error occurs during the invocation) |

## Initialization

The Jaeger plugin provides an interface for registering plugin and filters called `::trpc::jaeger::Init`. Users need to call this interface for initialization before starting the framework.

1. For server scenarios, users need to call it in the `TrpcApp::RegisterPlugins` function during service startup:

    ```cpp
    #include "trpc/tracing/jaeger/jaeger_tracing_api.h"

    class HelloworldServer : public ::trpc::TrpcApp {
     public:
      ...
      int RegisterPlugins() override {
        ::trpc::jaeger::Init();
        return 0;
      }
    };
    ```

2. For pure client scenarios, it needs to be called after initializing the framework configuration but before starting other modules of the framework:

    ```cpp
    #include "trpc/tracing/jaeger/jaeger_tracing_api.h"

    int main(int argc, char* argv[]) {
      ParseClientConfig(argc, argv);

      ::trpc::jaeger::Init();

      return ::trpc::RunInTrpcRuntime([]() { return Run(); });
    }
    ```

## Other features

In addition to the initialization interface, the Jaeger plugin provides several other interfaces for user usage, which are defined in the "[Jaeger API](./trpc/tracing/jaeger/jaeger_tracing_api.h)" file.

### Customize span content

You can retrieve the current Span from the ServerContext using the `::trpc::jaeger::GetSpan` interface:
```cpp
using JaegerTracingSpanPtr = std::shared_ptr<::opentracing::Span>;

/// @brief Gets the span.
/// @param context server context
/// @return Return the span saved in the context. Note that nullptr will be returned when there is no valid span in the
///         context.
JaegerTracingSpanPtr GetSpan(const ServerContextPtr& context);
```

After obtaining the Span, you can use the native [OpenTracing](https://github.com/opentracing/opentracing-cpp) API to setup the span. For example:
```cpp
::trpc::jaeger::JaegerTracingSpanPtr span = ::trpc::jaeger::GetSpan(context);
if (span) {
  span->SetTag("id", "1");
  span->Log({{"message1", "this is log content 0"}, {"message2", "this is log content 2"}});
}
```

## Notice

* In the proxy mode, it is necessary to invoke the framework's `MakeClientContext` interface to construct the `ClientContext` based on the `ServerContext`. Otherwise, the call relationship between the server and client will be lost, and a complete call chain cannot be formed.

# LICENSE

[LICENSE](LICENSE)

The copyright notice pertaining to the Tencent code in this repo was previously in the name of “THL A29 Limited.”  That entity has now been de-registered.  You should treat all previously distributed copies of the code as if the copyright notice was in the name of “Tencent.”
