cc_library(
    name = "emcc_tui",
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
        "//edit:emcc_edit",
        "//support:emcc_support",
    ],
)
