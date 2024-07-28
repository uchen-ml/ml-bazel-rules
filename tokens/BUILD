load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "tokens",
    srcs = ["main.cc"],
    # deps = [":lib"],
    copts = ["-std=c++20"],
    deps = [
        ":lib",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:flags",
        "@abseil-cpp//absl/log:initialize",
        "@abseil-cpp//absl/strings",
    ],
)

cc_library(
    name = "lib",
    srcs = [
        "bagger.cc",
        "tokens.cc",
    ],
    hdrs = [
        "bagger.h",
        "tokens.h",
    ],
    copts = ["-std=c++20"],
    deps = [
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/strings:str_format",
    ],
)
