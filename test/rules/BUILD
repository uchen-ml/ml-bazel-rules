load("//uchenml:bow.bzl", "bag_of_words")
load("//uchenml:datasets.bzl", "web_archive_dataset")
load("//uchenml:images.bzl", "resize_image")

web_archive_dataset(
    name = "grpc_cpp",
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
    max_size = 50000,
    md5 = "b2b3ae2a46a2500b57a7f9b85d0fa161",
    min_size = 1900,
    samples = 2000,
    seed = 10,
    strip_prefix_segments = 1,
    url = "https://github.com/grpc/grpc/archive/78cfebc3bd22ef20b60b208058e667eb720a9240.tar.gz",
)

bag_of_words(
    name = "codeart_bow",
    srcs = [":grpc_cpp"],
    vocab_size = 1000,
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
