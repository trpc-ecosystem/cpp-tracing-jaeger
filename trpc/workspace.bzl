"""This module contains some dependency"""

# buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def clean_dep(dep):
    return str(Label(dep))

# buildifier: disable=function-docstring-args
def tracing_jaeger_workspace(path_prefix = "", repo_name = "", **kwargs):
    """Build rules for the trpc project

    Note: The main idea is to determine the required version of dependent software during the build process
          by passing in parameters.
    Args:
        path_prefix: Path prefix.
        repo_name: Repository name of the dependency.
        kwargs: Keyword arguments, dictionary type, mainly used to specify the version and sha256 value of
                dependent software, where the key of the keyword is constructed by the `name + version`.
                eg: protobuf_ver,zlib_ver...
    Example:
        trpc_workspace(path_prefix="", repo_name="", protobuf_ver="xxx", protobuf_sha256="xxx", ...)
        Here, `xxx` is the specific specified version. If the version is not specified through the key,
        the default value will be used. eg: protobuf_ver = kwargs.get("protobuf_ver", "3.8.0")
    """

    # com_github_jaegertracing_jaeger_client_cpp
    native.new_local_repository(
        name = "com_github_jaegertracing_jaeger_client_cpp",
        path = "/usr/local",
        build_file = clean_dep("//third_party/com_github_jaegertracing_jaeger_client_cpp:jaegertracing.BUILD"),
    )

    # com_github_apache_thrift
    native.new_local_repository(
        name = "com_github_apache_thrift",
        path = "/usr/local",
        build_file = clean_dep("//third_party/com_github_apache_thrift:thrift.BUILD"),
    )

    # com_github_opentracing_opentracing_cpp
    com_github_opentracing_opentracing_cpp_ver = kwargs.get("com_github_opentracing_opentracing_cpp_ver", "1.6.0")
    com_github_opentracing_opentracing_cpp_sha256 = kwargs.get("com_github_opentracing_opentracing_cpp_sha256", "5b170042da4d1c4c231df6594da120875429d5231e9baa5179822ee8d1054ac3")
    com_github_opentracing_opentracing_cpp_urls = [
        "https://github.com/opentracing/opentracing-cpp/archive/refs/tags/v{ver}.tar.gz".format(ver = com_github_opentracing_opentracing_cpp_ver),
    ]
    http_archive(
        name = "com_github_opentracing_opentracing_cpp",
        sha256 = com_github_opentracing_opentracing_cpp_sha256,
        strip_prefix = "opentracing-cpp-{ver}".format(ver = com_github_opentracing_opentracing_cpp_ver),
        urls = com_github_opentracing_opentracing_cpp_urls,
    )
