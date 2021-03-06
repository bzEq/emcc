cc_library(
    name = "emcc_layout",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
        "-fblocks",
    ],
    linkopts = [
        "-fuse-ld=lld",
        "-lncurses",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//support:emcc_support",
    ],
)
