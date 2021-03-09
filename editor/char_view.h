#pragma once

#include <stddef.h>
#include <stdint.h>

namespace emcc::editor {

struct CharView {
  size_t point;
  wchar_t rune;
  uint8_t width;
  size_t length() const { return width; }
};

} // namespace emcc::editor
