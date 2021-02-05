cc_library(
    name = "emcc_editor",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
        "-Werror",
        "-flto",
    ],
    linkopts = [
        "-fuse-ld=lld",
        "-flto",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//support:emcc_support",
        "@fmt",
    ],
)
