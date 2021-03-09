#include "editor/buffer_view.h"

namespace emcc::editor {

void BufferView::Reset() {
  total_height_ = 0;
  lines_.clear();
  FillBufferView();
}

void BufferView::RePosition(size_t baseline) {
  if (baseline_ == baseline)
    return;
  if (baseline > baseline_ && baseline < baseline_ + lines_.size()) {
    size_t gap = baseline - baseline_;
    total_height_ = 0;
    for (size_t i = 0; i < lines_.size() - gap; ++i) {
      std::swap(lines_[i], lines_[i + gap]);
      total_height_ += lines_[i].height();
    }
    lines_.resize(lines_.size() - gap);
    baseline_ = baseline;
    FillBufferView();
  } else {
    baseline_ = baseline;
    Reset();
  }
}

void BufferView::Resize(size_t h, size_t w) {
  height_hint_ = h;
  width_ = w;
  total_height_ = 0;
  for (size_t i = 0; i < lines_.size(); ++i) {
    lines_[i].Resize(width_);
    total_height_ += lines_[i].height();
  }
  FillBufferView();
}

void BufferView::FillBufferView() {
  size_t current_line = baseline_ + lines_.size();
  while (total_height_ < height_hint_) {
    lines_.emplace_back(parent_, current_line, width_);
    total_height_ += lines_.back().height();
  }
}

} // namespace emcc::editor
