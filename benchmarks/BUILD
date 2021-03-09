COPTS = [
    "-std=c++17",
    "-O3",
    "-Wall",
]

LINKOPTS = [
    "-pthread",
    "-fuse-ld=lld",
]

cc_binary(
    name = "page_benchmark",
    srcs = ["page_benchmark.cc"],
    copts = COPTS + ["-fblocks"],
    linkopts = LINKOPTS + [
        "-lncurses",
    ],
    deps = [
        "//editor:emcc_editor",
        "//support:emcc_support",
        "//tui:emcc_tui",
    ],
)

cc_binary(
    name = "page_benchmark1",
    srcs = ["page_benchmark1.cc"],
    copts = COPTS + ["-fblocks"],
    linkopts = LINKOPTS + [
        "-lncurses",
    ],
    deps = [
        "//editor:emcc_editor",
        "//support:emcc_support",
        "//tui:emcc_tui",
    ],
)
