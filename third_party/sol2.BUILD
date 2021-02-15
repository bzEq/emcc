cc_library(
    name = "sol2",
    srcs = glob(["include/sol/**/*.hpp", "include/sol/**/*.h"]),
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    includes = [
        "include",
    ],
    linkopts = [
        "-fuse-ld=lld",
        "-ldl",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@LuaJIT//src:luajit",
    ],
)
