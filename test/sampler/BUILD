load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "path_matcher_test",
    size = "small",
    srcs = ["path_matcher.test.cc"],
    deps = [
        "//src/sampler:sampler_lib",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:initialize",
        "@googletest//:gtest",
    ],
)

