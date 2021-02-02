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
    deps = [
        "//support:emcc_support",
        "@fmt//:fmt",
    ],
    visibility = ["//visibility:public"],
)
