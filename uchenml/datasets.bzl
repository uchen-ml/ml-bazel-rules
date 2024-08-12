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
        "samples": attr.int(),
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

def web_archive_dataset(name, url, md5, strip_prefix_segments, include, exclude, extensions, min_size, max_size=0, samples=0, seed = 42):
    """
    Downloads a web archive and unpacks it.

    Args:
        url: The URL of the archive.
        name: The name of the target.
        strip_prefix_segments: The number of segments to strip from paths in the archive.
        include: A list of paths to include in the sample.
        exclude: A list of paths to exclude from the sample.
        extensions: A list of file extensions to include in the sample.
        min_size: The minimum size of files to include in the sample.
        max_size: The maximum size of files to include in the sample.
        samples: The number of samples to take.
        seed: The seed for the random number generator.
        md5: The MD5 checksum of the archive.
    """
    http_download(
        name = name + "_download",
        url = url,
        md5 = md5,
    )
    unpack(
        name = name + "_unpack",
        input = ":" + name + "_download",
        strip_prefix_segments = strip_prefix_segments,
    )
    sample(
        name = name,
        src = name + "_unpack",
        include = include,
        exclude = exclude,
        extensions = extensions,
        min_size = min_size,
        max_size = max_size,
        samples = samples,
        seed = seed,
    )
