"""Data set management rules."""

load("dataset/archives.bzl", "http_download_impl", "unpack_impl")
load("dataset/sample.bzl", "sample_impl")
load("providers.bzl", "DatasetInfo")

http_download = rule(
    implementation = http_download_impl,
    attrs = {
        "md5": attr.string(mandatory = False),
        "url": attr.string(mandatory = True),
        "_check_and_copy": attr.label(
            cfg = "exec",
            default = Label("//scripts:check_and_copy"),
            executable = True,
        ),
    },
)

sample = rule(
    implementation = sample_impl,
    attrs = {
        "src": attr.label(allow_files = True, mandatory = True),
        "include": attr.string_list(),
        "exclude": attr.string_list(),
        "extensions": attr.string_list(),
        "min_size": attr.int(),
        "max_size": attr.int(),
        "samples": attr.int(mandatory = True),
        "seed": attr.int(default = 42),
        "_sampler_bin": attr.label(
            default = Label("//src/sampler:sampler"),
            executable = True,
            cfg = "exec",
        ),
    },
    provides = [DefaultInfo, DatasetInfo],
)

unpack = rule(
    implementation = unpack_impl,
    attrs = {
        "input": attr.label(
            allow_single_file = [".tar.gz", ".zip"],
            mandatory = True,
        ),
        "strip_prefix_segments": attr.int(doc = "Number of segments to strip from paths"),
    },
)

def web_archive(name, url, md5, strip_prefix_segments):
    """
    Downloads a web archive and unpacks it.

    Args:
        name: The name of the target.
        url: The URL of the archive.
        md5: The MD5 checksum of the archive.
        strip_prefix_segments: The number of segments to strip from paths in the archive.
    """
    dl_task = name + "_download"
    http_download(
        name = dl_task,
        url = url,
        md5 = md5,
    )
    unpack(
        name = name,
        input = ":" + dl_task,
        strip_prefix_segments = strip_prefix_segments,
    )
