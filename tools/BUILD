cc_binary(
    name = "em",
    srcs = ["em.cc"],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-pthread",
    ],
    deps = [
        "//editor:emcc_editor",
    ],
)

cc_binary(
    name = "echo_lines",
    srcs = ["echo_lines.cc"],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-pthread",
    ],
    deps = [
        "//editor:emcc_editor",
    ],
)

cc_binary(
    name = "epoll_stdin",
    srcs = ["epoll_stdin.cc"],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-pthread",
    ],
    deps = [
        "//editor:emcc_editor",
        "//support:emcc_support",
        "@fmt//:fmt",
    ],
)

cc_binary(
    name = "monkey_typing",
    srcs = ["monkey_typing.cc"],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-pthread",
    ],
    deps = [
        "//editor:emcc_editor",
    ],
)
