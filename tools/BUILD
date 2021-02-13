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
    ],
)

cc_binary(
    name = "fuzz_line_buffer",
    srcs = ["fuzz_line_buffer.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//edit:emcc_edit",
    ],
)

cc_binary(
    name = "fuzz_mono_buffer",
    srcs = ["fuzz_mono_buffer.cc"],
    copts = COPTS,
    linkopts = LINKOPTS,
    deps = [
        "//edit:emcc_edit",
    ],
)
