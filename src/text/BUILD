load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "extractor",
    srcs = ["extractor.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:flags",
        "@abseil-cpp//absl/log:initialize",
        "@abseil-cpp//absl/strings",
    ],
)
