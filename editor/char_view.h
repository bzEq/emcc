#pragma once

#include <stddef.h>
#include <stdint.h>

namespace emcc::editor {

struct CharView {
  wchar_t character;
  uint8_t width;
  size_t length() const { return width; }
};

} // namespace emcc::editor
