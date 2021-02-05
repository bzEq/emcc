cc_library(
    name = "emcc_support",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
        "-fblocks",
        "-flto",
    ],
    linkopts = [
        "-flto",
        "-lBlocksRuntime",
        "-lre2",
        "-fuse-ld=lld",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@fmt",
    ],
)
