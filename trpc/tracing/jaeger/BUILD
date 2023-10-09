licenses(["notice"])

package(default_visibility = ["//visibility:public"])

filegroup(
    name = "test_yaml_files",
    srcs = glob([
        "testing/*.yaml",
    ]),
)

cc_library(
    name = "jaeger_tracing_conf",
    srcs = ["jaeger_tracing_conf.cc"],
    hdrs = ["jaeger_tracing_conf.h"],
    deps = [
        "@com_github_jbeder_yaml_cpp//:yaml-cpp",
        "@trpc_cpp//trpc/util/log:logging",
    ],
)

cc_library(
    name = "jaeger_tracing_conf_parser",
    hdrs = ["jaeger_tracing_conf_parser.h"],
    deps = [
        ":jaeger_tracing_conf",
    ],
)

cc_test(
    name = "jaeger_tracing_conf_test",
    srcs = ["jaeger_tracing_conf_test.cc"],
    deps = [
        ":jaeger_tracing_conf",
        ":jaeger_tracing_conf_parser",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "jaeger_common",
    srcs = ["jaeger_common.cc"],
    hdrs = ["jaeger_common.h"],
    deps = [
        "@com_github_jaegertracing_jaeger_client_cpp//:jaegertracing",
        "@trpc_cpp//trpc/client:client_context",
        "@trpc_cpp//trpc/tracing:tracing_filter_index",
    ],
)

cc_test(
    name = "jaeger_common_test",
    srcs = ["jaeger_common_test.cc"],
    deps = [
        ":jaeger_common",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@trpc_cpp//trpc/codec/trpc:trpc_client_codec",
    ],
)

cc_library(
    name = "jaeger_tracing",
    srcs = ["jaeger_tracing.cc"],
    hdrs = ["jaeger_tracing.h"],
    deps = [
        ":jaeger_common",
        ":jaeger_tracing_conf",
        ":jaeger_tracing_conf_parser",
        "@com_github_jaegertracing_jaeger_client_cpp//:jaegertracing",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/log:logging",
        "@trpc_cpp//trpc/tracing",
    ],
)

cc_test(
    name = "jaeger_tracing_test",
    srcs = ["jaeger_tracing_test.cc"],
    data = [":test_yaml_files"],
    deps = [
        ":jaeger_tracing",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "jaeger_text_map_carrier",
    srcs = ["jaeger_text_map_carrier.cc"],
    hdrs = ["jaeger_text_map_carrier.h"],
    deps = [
        "@com_github_opentracing_opentracing_cpp//:opentracing",
        "@com_google_protobuf//:protobuf",
        "@trpc_cpp//trpc/util/log:logging",
    ],
)

cc_test(
    name = "jaeger_text_map_carrier_test",
    srcs = ["jaeger_text_map_carrier_test.cc"],
    deps = [
        ":jaeger_text_map_carrier",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "jaeger_client_filter",
    srcs = ["jaeger_client_filter.cc"],
    hdrs = ["jaeger_client_filter.h"],
    deps = [
        ":jaeger_common",
        ":jaeger_text_map_carrier",
        ":jaeger_tracing",
        "@trpc_cpp//trpc/client:client_context",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/filter",
        "@trpc_cpp//trpc/tracing:tracing_factory",
    ],
)

cc_test(
    name = "jaeger_client_filter_test",
    srcs = ["jaeger_client_filter_test.cc"],
    data = [":test_yaml_files"],
    deps = [
        ":jaeger_client_filter",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@trpc_cpp//trpc/codec/trpc:trpc_client_codec",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/tracing:tracing_factory",
    ],
)

cc_library(
    name = "jaeger_server_filter",
    srcs = ["jaeger_server_filter.cc"],
    hdrs = ["jaeger_server_filter.h"],
    deps = [
        ":jaeger_common",
        ":jaeger_text_map_carrier",
        ":jaeger_tracing",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/filter",
        "@trpc_cpp//trpc/server:server_context",
        "@trpc_cpp//trpc/tracing:tracing_factory",
        "@trpc_cpp//trpc/tracing:tracing_filter_index",
    ],
)

cc_test(
    name = "jaeger_server_filter_test",
    srcs = ["jaeger_server_filter_test.cc"],
    data = [":test_yaml_files"],
    deps = [
        ":jaeger_server_filter",
        ":jaeger_tracing",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@trpc_cpp//trpc/codec:codec_manager",
        "@trpc_cpp//trpc/codec/trpc/testing:trpc_protocol_testing",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/proto/testing:cc_helloworld_proto",
        "@trpc_cpp//trpc/serialization:trpc_serialization",
        "@trpc_cpp//trpc/server/rpc:rpc_service_impl",
        "@trpc_cpp//trpc/server/testing:server_context_testing",
        "@trpc_cpp//trpc/tracing:tracing_factory",
    ],
)

cc_library(
    name = "jaeger_tracing_api",
    srcs = ["jaeger_tracing_api.cc"],
    hdrs = ["jaeger_tracing_api.h"],
    deps = [
        ":jaeger_client_filter",
        ":jaeger_server_filter",
        ":jaeger_tracing",
        "@trpc_cpp//trpc/common:trpc_plugin",
        "@trpc_cpp//trpc/server:server_context",
        "@trpc_cpp//trpc/tracing:tracing_factory",
        "@trpc_cpp//trpc/tracing:tracing_filter_index",
    ],
)

cc_test(
    name = "jaeger_tracing_api_test",
    srcs = ["jaeger_tracing_api_test.cc"],
    data = [":test_yaml_files"],
    deps = [
        ":jaeger_tracing",
        ":jaeger_tracing_api",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
        "@trpc_cpp//trpc/client/testing:service_proxy_testing",
        "@trpc_cpp//trpc/codec/trpc/testing:trpc_protocol_testing",
        "@trpc_cpp//trpc/common/config:trpc_config",
        "@trpc_cpp//trpc/proto/testing:cc_helloworld_proto",
        "@trpc_cpp//trpc/server/rpc:rpc_service_impl",
        "@trpc_cpp//trpc/server/testing:server_context_testing",
        "@trpc_cpp//trpc/tracing:tracing_factory",
        "@trpc_cpp//trpc/tracing:tracing_filter_index",
    ],
)
