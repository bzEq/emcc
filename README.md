# [WIP] emcc
An Emacs in C++.

## Prerequisites
### Bazel build system
Bazel can be installed following https://docs.bazel.build/versions/master/install-ubuntu.html

### Packages
```
sudo apt install clang libgtest-dev libre2-dev libblocksruntime-dev
```

## Run all tests
```
CC=clang bazel test unittests:*
```

## Build all tools
```
CC=clang bazel build tools:*
```
