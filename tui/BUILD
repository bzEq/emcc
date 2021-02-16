cc_library(
    name = "emcc_tui",
    srcs = glob(["*.cc"], exclude=["buffer_view.cc"]),
    hdrs = glob(["*.h"], exclude=["buffer_view.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
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
