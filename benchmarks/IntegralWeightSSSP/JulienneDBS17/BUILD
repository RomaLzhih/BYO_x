licenses(["notice"])

cc_library(
    name = "wBFS",
    hdrs = ["wBFS.h"],
    deps = [
        "//gbbs",
        "//gbbs:bucket",
    ],
)

cc_library(
    name = "wBFS_unweighted",
    hdrs = ["wBFS_unweighted.h"],
    deps = [
        "//gbbs",
        "//gbbs:bucket",
    ],
)

cc_binary(
    name = "wBFS_main",
    srcs = ["wBFS.cc"],
    deps = [":wBFS"],
)

package(
    default_visibility = ["//visibility:public"],
)
