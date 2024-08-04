load("//uchenml:defs.bzl", "http_download", "sample", "unpack")

http_download(
    name = "download",
    url = "https://github.com/grpc/grpc/archive/78cfebc3bd22ef20b60b208058e667eb720a9240.tar.gz",
    md5 = "b2b3ae2a46a2500b57a7f9b85d0fa161",
)

unpack(
    name = "main_data_set",
    input = ":download",
    strip_prefix_segments = 1,
)

sample(
    name = "some_src",
    dataset = ":main_data_set",
    include_dirs = ["src", "test"],
    extensions = ["cc", "h"],
)