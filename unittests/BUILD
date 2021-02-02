cc_test(
    name = "rope_test",
    srcs = [
        "rope_test.cc",
    ],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-lgtest_main",
        "-lgtest",
        "-pthread",
    ],
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "rope_benchmark",
    srcs = [
        "rope_benchmark.cc",
    ],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-lgtest_main",
        "-lgtest",
        "-pthread",
    ],
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
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-lgtest_main",
        "-lgtest",
        "-pthread",
    ],
    deps = [
        "//support:emcc_support",
        "@fmt//:fmt",
        "@trie//:trie",
    ],
)

cc_test(
    name = "defer_test",
    srcs = [
        "defer_test.cc",
    ],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
        "-fblocks",
    ],
    linkopts = [
        "-lgtest_main",
        "-lgtest",
        "-pthread",
    ],
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "chan_test",
    srcs = [
        "chan_test.cc",
    ],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-lgtest_main",
        "-lgtest",
        "-pthread",
    ],
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "size_limit_queue_test",
    srcs = [
        "size_limit_queue_test.cc",
    ],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-lgtest_main",
        "-lgtest",
        "-pthread",
    ],
    deps = [
        "//support:emcc_support",
    ],
)

cc_test(
    name = "line_buffer_test",
    srcs = [
        "line_buffer_test.cc",
    ],
    copts = [
        "-std=c++17",
        "-O3",
        "-Wall",
    ],
    linkopts = [
        "-lgtest_main",
        "-lgtest",
        "-pthread",
    ],
    deps = [
        "//editor:emcc_editor",
    ],
)
