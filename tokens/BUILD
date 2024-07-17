load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")

package(default_visibility = ["//visibility:public"])

cc_binary(
  name = "tokens",
  srcs = ["main.cc"],
  deps = [":lib"],
)

cc_library(
  name = "lib",
  srcs = ["tokens.cc"],
  hdrs = ["tokens.h"],
)