load("//uchenml:bow.bzl", "bag_of_words")
load("//uchenml:datasets.bzl", "web_archive_dataset")

web_archive_dataset(
    name = "grpc_cpp",
    url = "https://github.com/grpc/grpc/archive/78cfebc3bd22ef20b60b208058e667eb720a9240.tar.gz",
    md5 = "b2b3ae2a46a2500b57a7f9b85d0fa161",
    include = [
        "src",
        "test",
    ],
    exclude = [
        "src/ruby",
        "src/compiler/",
        "src/core/ext/upb-gen",
        "src/core/ext/upbdefs-gen"
    ],
    extensions = [
        "cc",
        "h",
    ],
    min_size = 500,
    max_size = 2000,
    samples = 10000,
    seed = 10,
    strip_prefix_segments = 1,
)

bag_of_words(
    name = "codeart_bow",
    srcs = [":grpc_cpp"],
    vocab_size = 1000,
)
