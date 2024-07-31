load("//uchenml:defs.bzl", "http_download", "unpack")

http_download(
    name = "download",
    url = "https://github.com/grpc/grpc/archive/78cfebc3bd22ef20b60b208058e667eb720a9240.tar.gz",
    md5 = "5eac4a909fc84129d4f5b7e7f4ee2b70",
)

unpack(
    name = "main_data_set",
    input = ":download",
)