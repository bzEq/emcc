cc_library(
    name = "emcc_support",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
        "-fblocks",
    ],
    linkopts = [
        "-lre2",
        "-fuse-ld=lld",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@fmt",
    ],
)
