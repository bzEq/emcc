cc_library(
    name = "emcc_editor",
    srcs = glob(["*.cc"]),
    hdrs = glob(["*.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
        "-Werror",
    ],
    linkopts = [
        "-fuse-ld=lld",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//support:emcc_support",
        "@fmt",
    ],
)
