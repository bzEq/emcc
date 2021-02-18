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
        "//edit:emcc_edit",
        "//tui:emcc_tui",
    ],
)
