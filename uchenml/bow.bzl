"""Bag-of-words construction rules"""

load("bow/bow_impl.bzl", "bag_of_words_impl")

bag_of_words = rule(
    implementation = bag_of_words_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = True, mandatory = True),
        "vocab_size": attr.int(mandatory = True),
        "_bow_tool": attr.label(
            default = Label("//src/bow"),
            cfg = "exec",
            executable = True,
            allow_single_file = True,
        ),
    },
)
