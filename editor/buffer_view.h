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

  size_t NumLines() const { return lines_.size(); }

  size_t NumRows() const { return total_height_; }

  class row_iterator {
  public:
    row_iterator &operator++() {
      if (lineno_ >= parent_.lines_.size())
        return *this;
      auto &lv = parent_.lines_[lineno_];
      ++segno_;
      if (segno_ >= lv.height()) {
        ++lineno_;
        segno_ = 0;
      }
      return *this;
    }
    bool operator==(const row_iterator &other) const {
      return &parent_ == &other.parent_ && lineno_ == other.lineno_ &&
             segno_ == other.segno_;
    }
    bool operator!=(const row_iterator &other) const {
      return !(*this == other);
    }

    emcc::iterator_range<LineView::iterator> operator*() {
      return parent_.lines_[lineno_].GetSegment(segno_);
    }

  private:
    friend class BufferView;
    row_iterator(BufferView &parent) : parent_(parent), lineno_(0), segno_(0) {}

    BufferView &parent_;
    size_t lineno_, segno_;
  };

  row_iterator begin() { return row_iterator(*this); }

  row_iterator end() {
    row_iterator it(*this);
    it.lineno_ = lines_.size();
    return it;
  }

private:
  friend class row_iterator;
  void FillBufferView();

  MonoBuffer *parent_;
  size_t height_hint_, width_, baseline_, total_height_;
  std::vector<LineView> lines_;
};

} // namespace emcc::editor
