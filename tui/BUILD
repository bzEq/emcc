cc_library(
    name = "emcc_tui",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-fuse-ld=lld",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//support:emcc_support",
        "//editor:emcc_editor",
    ],
)
