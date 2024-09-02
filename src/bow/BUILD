load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "bow",
    srcs = ["main.cc"],
    deps = [
        ":bowlib",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:flags",
        "@abseil-cpp//absl/log:initialize",
    ],
)

cc_library(
    name = "bowlib",
    srcs = ["tokens.cc"],
    hdrs = ["tokens.h"],
    deps = [
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/strings:str_format",
    ],
)

cc_library(
    name = "extract_words",
    srcs = ["extract_words.cc"],
    hdrs = ["extract_words.h"],
    deps = [
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/strings",
    ],
)
