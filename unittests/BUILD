COPTS = [
    "-std=c++17",
    "-O3",
    "-Wall",
    "-fblocks",
]

LINKOPTS = [
    "-pthread",
    "-fuse-ld=lld",
    "-lgtest_main",
    "-lgtest",
    "-lBlocksRuntime",
]

cc_test(
    name = "rope_test",
    srcs = [
        "rope_test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "rope_benchmark",
    srcs = [
        "rope_benchmark.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "other_test",
    srcs = [
        "result_test.cc",
        "trie_test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//support:emcc_support",
        "@fmt",
        "@trie",
    ],
)

cc_test(
    name = "defer_test",
    srcs = [
        "defer_test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "chan_test",
    srcs = [
        "chan_test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "size_limit_queue_test",
    srcs = [
        "size_limit_queue_test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "line_buffer_test",
    srcs = [
        "line_buffer_test.cc",
    ],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//editor:emcc_editor",
    ],
)
