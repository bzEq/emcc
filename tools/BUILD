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
    ],
)

cc_binary(
    name = "echo_lines",
    srcs = ["echo_lines.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//editor:emcc_editor",
    ],
)

cc_binary(
    name = "epoll_stdin",
    srcs = ["epoll_stdin.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//editor:emcc_editor",
        "//support:emcc_support",
        "@fmt",
    ],
)

cc_binary(
    name = "monkey_typing",
    srcs = ["monkey_typing.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//editor:emcc_editor",
    ],
)

cc_binary(
    name = "fuzz_line_buffer",
    srcs = ["fuzz_line_buffer.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//editor:emcc_editor",
    ],
)
