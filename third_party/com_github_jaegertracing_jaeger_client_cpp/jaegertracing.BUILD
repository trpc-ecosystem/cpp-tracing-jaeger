package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "jaegertracing",
    srcs = glob(["lib*/libjaegertracing.a"]),
    hdrs = glob(["include/jaegertracing/**/*.h"]),
    includes = ["include"],
    deps = [
        "@com_github_apache_thrift//:thrift",
        "@com_github_opentracing_opentracing_cpp//:opentracing",
    ],
)
