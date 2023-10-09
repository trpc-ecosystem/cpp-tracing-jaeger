[English](./README.md)

[TOC]

# 前言

[Jaeger](https://www.jaegertracing.io/)是一个开源的端到端分布式追踪系统，遵循[OpenTracing](https://opentracing.io/)标准。为了方便用户对接Jaeger系统，我们提供了Jaeger调用链插件，实现Jaeger调用链数据的便捷采集和上报。

# 使用说明

详细的使用例子可以参考：[Jaeger examples](./examples/)。

## 引入依赖

### Bazel

1. 引入仓库

    在项目的`WORKSPACE`文件中，引入`cpp-tracing-jaeger`仓库及其依赖：
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

    [jaeger-client-cpp](https://github.com/jaegertracing/jaeger-client-cpp)依赖的第三方库很多，完全基于源码进行bazel编译很麻烦。为了简化编译流程，我们通过本地库的方式来引入jaeger-client-cpp（v0.9.0版本）和其必须的依赖库thrift（v0.12.0-p0版本）。编译之前用户需要先将jaeger-client-cpp和thrift安装到/usr/local目录下。

2. 引入插件

    在需要用到Jaeger的目标中引入“`trpc/tracing/jaeger:jaeger_tracing_api`”依赖。例如：
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

暂不支持。

## 插件配置

要使用Jaeger插件，必须在框架配置文件中加上插件配置：
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

插件支持了jaeger-client-cpp v0.9.0版本的全部yaml配置参数。在这里我们只对插件特有的配置项和jaeger-client-cpp的部分关键配置项进行说明，其他配置参数请参考[jaeger-client-cpp的配置](https://github.com/jaegertracing/jaeger-client-cpp/blob/v0.9.0/src/jaegertracing/Config.h)。

| 配置项 | 类型 | 取值 | 说明 |
| ------ | ------ | ------ | ------ |
| enable_logger | bool | 默认为false | 控制是否打印Span的信息 |
| disabled | bool | 默认为false | 控制是否关闭调用链信息采集和上报 |
| sampler | 映射（Mapping） | “type”默认取“const”，“param”默认取1，表示全采样 | 控制[采样策略](https://www.jaegertracing.io/docs/1.21/sampling/) |
| reporter:localAgentHostPort | string | 默认为“127.0.0.1:6831” | 调用链信息的上报地址 |

## 启用客户端拦截器

只需要在框架的客户端配置中加上Jaeger拦截器即可：
```yaml
client:
  filter:
    - jaeger
```

客户端拦截器创建的Span的`operationName`是被调的接口名，从ClientContext::GetFuncName获取。另外拦截器会自动往Span中设置以下数据：

| 标签 | 类型 | 值 |
| ------ | ------ | ------ |
| span.kind | Tag | "client" |
| peer.service | Tag | 被调服务名 |
| peer.hostname | Tag | 被调容器名 |
| peer.ipv4/peer.ipv6 | Tag | 被调ip地址 |
| peer.port | Tag | 被调端口 |
| _trace_ext.response.size | Tag | 响应包大小（单位：bytes） |
| _trace_ext.namespace | Tag | namespace |
| _trace_ext.envname | Tag | env |
| _trace_ext.ret | Tag | 调用的错误码 |
| error | Tag | true（调用发生错误时才会设置） |
| event | Log | "error"（调用发生错误时才会设置） |
| message | Log | 错误信息（调用发生错误时才会设置） |

## 启用服务端拦截器

只需要在框架的服务端配置中加上Jaeger拦截器即可：
```yaml
server:
  filter:
    - jaeger
```

服务端拦截器创建的Span的`operationName`是被调的接口名，从ServerContext::GetFuncName获取。另外拦截器会自动往Span中设置以下数据：

| 标签 | 类型 | 值 |
| ------ | ------ | ------ |
| span.kind | Tag | "server" |
| _trace_ext.request.size | Tag | 请求包大小（单位：bytes） |
| _trace_ext.namespace | Tag | namespace |
| _trace_ext.envname | Tag | env |
| _trace_ext.ret | Tag | 调用的错误码 |
| error | Tag | true（调用发生错误时才会设置） |
| event | Log | "error"（调用发生错误时才会设置） |
| message | Log | 错误信息（调用发生错误时才会设置） |

## 初始化

Jaeger插件提供了插件和拦截器注册的接口`::trpc::jaeger::Init`，用户需要在框架启动前调用该接口进行初始化。

1. 对于服务端场景，用户需要在服务启动的`TrpcApp::RegisterPlugins`函数中调用：

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

2. 对于纯客户端场景，需要在启动框架配置初始化后，框架其他模块启动前调用：

    ```cpp
    #include "trpc/tracing/jaeger/jaeger_tracing_api.h"

    int main(int argc, char* argv[]) {
      ParseClientConfig(argc, argv);

      ::trpc::jaeger::Init();

      return ::trpc::RunInTrpcRuntime([]() { return Run(); });
    }
    ```

## 其他功能

除了初始化接口外，Jaeger插件提供了一些其他的接口供用户使用，它们定义在“[Jaeger API](./trpc/tracing/jaeger/jaeger_tracing_api.h)”文件中。

### 自定义Span内容

可以通过`::trpc::jaeger::GetSpan`接口从ServerContext中取出当前调用的Span：
```cpp
using JaegerTracingSpanPtr = std::shared_ptr<::opentracing::Span>;

/// @brief Gets the span.
/// @param context server context
/// @return Return the span saved in the context. Note that nullptr will be returned when there is no valid span in the
///         context.
JaegerTracingSpanPtr GetSpan(const ServerContextPtr& context);
```

在获取到Span后，调用[OpenTracing](https://github.com/opentracing/opentracing-cpp)原生的API对Span进行设置。例如：
```cpp
::trpc::jaeger::JaegerTracingSpanPtr span = ::trpc::jaeger::GetSpan(context);
if (span) {
  span->SetTag("id", "1");
  span->Log({{"message1", "this is log content 0"}, {"message2", "this is log content 2"}});
}
```

## 注意事项

* 在中转模式下，`ClientContext`需要调用框架的`MakeClientContext`接口，根据`ServerContext`来构造。否则服务端和客户端之间的调用关系会丢失，无法构成完整的调用链。
