load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "combine_test",
    size = "small",
    srcs = ["combine.test.cc"],
    deps = [
        "//src/tokens:lib",
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
        "//src/tokens:lib",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:initialize",
        "@googletest//:gtest_main",
    ],
)
