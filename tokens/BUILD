load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "tokens",
    srcs = ["main.cc"],
    # deps = [":lib"],
    copts = ["-std=c++20"],
    deps = [
        # "@abseil-cpp//absl/log",
        # "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
        # "@abseil-cpp//absl/strings:str_format",
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
    deps = [
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:check",
        "@abseil-cpp//absl/strings",
        "@abseil-cpp//absl/strings:str_format",
    ],
    copts = ["-std=c++20"],
)
