cc_library(
    name = "emcc_support",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
        "-Werror",
        "-fblocks",
    ],
    linkopts = [
        "-lBlocksRuntime",
        "-lre2",
    ],
    deps = [
        "@fmt//:fmt",
    ],
    visibility = ["//visibility:public"],
)
