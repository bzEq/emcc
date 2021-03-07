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
    name = "em",
    srcs = ["em.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//editor:emcc_editor",
        "//tui:emcc_tui",
    ],
)
