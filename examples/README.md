# Jaeger tracing demo

The server invocation relationship is as follows:

client <---> proxy <---> server

Since the usage of the Jaeger plugin is independent of the runtime type, this demo will only use the fiber mode for demonstration purposes.


## Usage

We can use the following command to view the directory tree.
```shell
$ tree examples/
examples/
├── client
│   ├── BUILD
│   ├── client.cc
│   └── trpc_cpp_fiber.yaml
├── proxy
│   ├── BUILD
│   ├── forward.proto
│   ├── forward_server.cc
│   ├── forward_service.cc
│   ├── forward_service.h
│   └── trpc_cpp_fiber.yaml
├── README.md
├── run.sh
└── server
    ├── BUILD
    ├── greeter_service.cc
    ├── greeter_service.h
    ├── helloworld.proto
    ├── helloworld_server.cc
    └── trpc_cpp_fiber.yaml
```

* Compilation

We can run the following command to compile the demo.

```shell
$ bazel build //examples/...
```

* Run the server/proxy program

We can run the following command to start the server and proxy program.

```shell
$ ./bazel-bin/examples/server/helloworld_server --config=examples/server/trpc_cpp_fiber.yaml
```

```shell
$ ./bazel-bin/examples/proxy/forward_server --config=examples/proxy/trpc_cpp_fiber.yaml
```

* Run the client program

We can run the following command to start the client program.

```shell
$ ./bazel-bin/examples/client/client --config=examples/client/trpc_cpp_fiber.yaml
```

* View the tracing data

In order to see the reporting results in Jaeger, you need to set up a Jaeger system including Jaeger UI, Collector, Jaeger Query, and Agent. The data should be reported to the Agent and can be viewed through Jaeger UI or Jaeger Query.

Please note that this demo assumes that the tracing data is reported to 127.0.0.1:6831 by default. If you want to report to a different address, you can modify the "jaeger:reporter:localAgentHostPort" configuration item to achieve this.
