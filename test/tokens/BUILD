load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "combine_test",
    size = "small",
    srcs = ["combine.test.cc"],
    deps = [
        "//src/bow:bowlib",
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
        "//src/bow:bowlib",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:initialize",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "word_extractor_test",
    size = "small",
    srcs = ["word_extractor.test.cc"],
    deps = [
        "//src/bow:extract_words",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:initialize",
        "@googletest//:gtest_main",
    ],
)