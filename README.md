# [WIP] emcc
An Emacs in C++.

## Prerequisites
### Clang supports c++17
Clang can be downloaded via https://releases.llvm.org

### Bazel build system
Bazel can be installed following https://docs.bazel.build/versions/master/install-ubuntu.html

### Packages
```
sudo apt install libgtest-dev libre2-dev libblocksruntime-dev
```

## Run all tests
```
CC=clang bazel test unittests:*
```

## Build all tools
```
CC=clang bazel build tools:*
```
