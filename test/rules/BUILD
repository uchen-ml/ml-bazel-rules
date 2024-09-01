load("//uchenml:bow.bzl", "bag_of_words")
load("//uchenml:datasets.bzl", "sample", "web_archive")
load("//uchenml:images.bzl", "resize_image")
load("//uchenml:text.bzl", "extract_text")

web_archive(
    name = "grpc_cpp_dl",
    md5 = "b2b3ae2a46a2500b57a7f9b85d0fa161",
    strip_prefix_segments = 1,
    url = "https://github.com/grpc/grpc/archive/78cfebc3bd22ef20b60b208058e667eb720a9240.tar.gz",
)

sample(
    name = "grpc_cpp",
    src = ":grpc_cpp_dl",
    include = [
        "src",
        "test",
    ],
    exclude = [
        "src/ruby",
        "src/compiler/",
        "src/core/ext/upb-gen",
        "src/core/ext/upbdefs-gen",
    ],
    extensions = [
        "cc",
        "h",
    ],
    max_size = 20000,
    min_size = 1000,
    samples = 128,
)

extract_text(
    name = "grpc_cpp_middle_text",
    srcs = [":grpc_cpp"],
    min_length = 1000,
    max_length = 3000,
)

bag_of_words(
    name = "codeart_bow",
    srcs = [":grpc_cpp"],
    size = 1000,
)

resize_image(
    name = "resize_logo",
    srcs = [
        "uchen-logo.svg",
        "uchen-logo-text.svg",
    ],
    height = 250,
    width = 250,
)
