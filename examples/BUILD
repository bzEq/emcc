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
    name = "echo_lines",
    srcs = ["echo_lines.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//edit:emcc_edit",
    ],
)

cc_binary(
    name = "echo_file",
    srcs = ["echo_file.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//edit:emcc_edit",
    ],
)

cc_binary(
    name = "epoll_stdin",
    srcs = ["epoll_stdin.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//edit:emcc_edit",
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
        "//edit:emcc_edit",
    ],
)

cc_binary(
    name = "play_ncurses",
    srcs = ["play_ncurses.cc"],
    copts = COPTS,
    linkopts = LINKOPTS + [
        "-lncurses",
    ],
)

cc_binary(
    name = "show_page",
    srcs = ["show_page.cc"],
    copts = COPTS + ['-fblocks'],
    linkopts = LINKOPTS + [
        "-lncurses",
    ],
    deps = [
        "//edit:emcc_edit",
        "//support:emcc_support",
        "//tui:emcc_tui",
    ],
)

cc_binary(
    name = "fill_page",
    srcs = ["fill_page.cc"],
    copts = COPTS,
    linkopts = LINKOPTS + [
        "-lncurses",
    ],
    deps = [
        "//edit:emcc_edit",
        "//support:emcc_support",
        "//tui:emcc_tui",
    ],
)
