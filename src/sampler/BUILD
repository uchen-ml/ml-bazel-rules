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
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/status:statusor",
    ],
)

cc_library(
    name = "sampler_lib",
    srcs = ["path_matcher.cc"],
    hdrs = ["path_matcher.h"],
    visibility = ["//visibility:public"],
    strip_include_prefix = "//src",
)