load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "bagger_test",
    size = "small",
    srcs = ["bagger.test.cc"],
    deps = [
        "//tokens:lib",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:initialize",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "tokens_test",
    size = "small",
    srcs = ["tokens.test.cc"],
    deps = [
        "//tokens:lib",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:initialize",
        "@googletest//:gtest_main",
    ],
)
