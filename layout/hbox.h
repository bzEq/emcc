#pragma once

#include <stddef.h>

namespace emcc::layout {

struct HBox {
  HBox(size_t len, void *data = nullptr) : len(len), data(data) {}
  size_t len;
  void *data;
};

} // namespace emcc::layout
