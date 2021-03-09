#pragma once

#include "editor/char_view.h"
#include "editor/line_view.h"
#include "editor/mono_buffer.h"

namespace emcc::editor {

class BufferView {
public:
  explicit BufferView(MonoBuffer *parent, size_t h, size_t w)
      : parent_(parent), height_(h), width_(w) {}

private:
  MonoBuffer *parent_;
  size_t height_, width_;
};

} // namespace emcc::editor
