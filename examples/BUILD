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
        "//editor:emcc_editor",
    ],
)

cc_binary(
    name = "echo_file",
    srcs = ["echo_file.cc"],
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
    name = "show_page1",
    srcs = ["show_page1.cc"],
    copts = COPTS + ["-fblocks"],
    linkopts = LINKOPTS + [
        "-lncursesw",
        "-lncurses",
    ],
    deps = [
        "//editor:emcc_editor",
        "//support:emcc_support",
        "//tui:emcc_tui",
    ],
)

cc_binary(
    name = "play_luajit_sol2",
    srcs = ["play_luajit_sol2.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "@sol2",
    ],
)

cc_binary(
    name = "play_terminal",
    srcs = ["play_terminal.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//tui:emcc_tui",
    ],
)

cc_binary(
    name = "auto_scroll_page",
    srcs = ["auto_scroll_page.cc"],
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
