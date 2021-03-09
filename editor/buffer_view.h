#pragma once

#include "editor/char_view.h"
#include "editor/line_view.h"
#include "editor/mono_buffer.h"

namespace emcc::editor {

class BufferView {
public:
  explicit BufferView(MonoBuffer *parent, size_t h, size_t w)
      : parent_(parent), height_hint_(h), width_(w), baseline_(0),
        total_height_(0) {
    Reset();
  }

  void Reset();
  void RePosition(size_t baseline);
  void Resize(size_t h, size_t w);

private:
  void FillBufferView();

  MonoBuffer *parent_;
  size_t height_hint_, width_, baseline_, total_height_;
  std::vector<LineView> lines_;
};

} // namespace emcc::editor
