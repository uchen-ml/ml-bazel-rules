load("//uchenml:defs.bzl", "web_archive_dataset")

web_archive_dataset(
    name = "grpc_cpp",
    md5 = "b2b3ae2a46a2500b57a7f9b85d0fa161",
    url = "https://github.com/grpc/grpc/archive/78cfebc3bd22ef20b60b208058e667eb720a9240.tar.gz",
    strip_prefix_segments = 1,
    include = [
        "src",
    ],
    exclude = ["src/ruby", "src/compiler/"],
    extensions = [
        "cc",
        "h",
    ],
    max_size = 5000,
    min_size = 3000,
    samples = 20,
    seed = 10,
)
