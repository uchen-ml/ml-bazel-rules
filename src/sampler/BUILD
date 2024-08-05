load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "sampler",
    srcs = ["main.cc"],
    visibility = ["//visibility:public"],
    deps = [
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:flags",
        "@abseil-cpp//absl/log:initialize",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings",
    ],
)

cc_library(
    name = "sampler_lib",
    srcs = ["path_matcher.cc"],
    hdrs = ["path_matcher.h"],
    strip_include_prefix = "//src",
    visibility = ["//visibility:public"],
    deps = [
        "@abseil-cpp//absl/algorithm:container",
        "@abseil-cpp//absl/strings",
    ],
)
