"""Common definitions for UchenML rules for training data management."""

# TODO: Git, sample

load("archives.bzl", "http_download_impl", "unpack_impl")
load("sample.bzl", "sample_impl")

http_download = rule(
    implementation = http_download_impl,
    attrs = {
        "md5": attr.string(mandatory = False),
        "output": attr.output(mandatory = False),
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
        "dataset": attr.label(allow_files = True, mandatory = True),
        "_sampler_bin": attr.label(
            default = Label("//src/sampler:sampler"),
            executable = True,
            cfg = "exec",
        ),
        "include_dirs": attr.string_list(),
        "extensions": attr.string_list(),
    },
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
