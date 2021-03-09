#pragma once

#include "editor/char_view.h"
#include "editor/mono_buffer.h"
#include "support/misc.h"
#include "support/utf8.h"
#include "support/wcwidth.h"

#include <stddef.h>
#include <stdint.h>

namespace emcc::editor {

// Break a logical line in MonoBuffer into segments.
class LineView {
public:
  LineView() = default;
  LineView(MonoBuffer *parent, size_t lineno, size_t width)
      : width_(width), parent_(parent) {
    Reset(lineno);
  }

  void Reset(size_t lineno);
  size_t height() const { return segment_index_.size(); }

  void Resize(size_t width) {
    width_ = width;
    ReCompute(0);
  }

  template <typename... Args>
  void Insert(int y, int x, Args &&...args) {
    if (segment_index_.empty()) {
      Append(std::forward<Args>(args)...);
      return;
    }
    y = std::min((size_t)y, segment_index_.size() - 1);
    x = std::min((size_t)x, NumCharViews(y));
    size_t i = segment_index_[y] + x;
    hlist_.emplace(hlist_.begin() + i, std::forward<Args>(args)...);
    ReCompute(y);
  }

  template <typename... Args>
  void Append(Args &&...args) {
    hlist_.emplace_back(std::forward<Args>(args)...);
    if (segment_index_.empty())
      ReCompute(0);
    else
      ReCompute(segment_index_.size() - 1);
  }

  size_t NumCharViews(size_t seg) const {
    if (seg >= segment_index_.size())
      return 0;
    if (seg == segment_index_.size() - 1)
      return hlist_.size() - segment_index_[seg];
    return segment_index_[seg + 1] - segment_index_[seg];
  }

  class iterator {
  public:
    friend class LineView;
    iterator &operator++() {
      ++index_;
      return *this;
    }
    bool operator==(const iterator &other) const {
      return &parent_ == &other.parent_ && index_ == other.index_;
    }
    bool operator!=(const iterator &other) const { return !(*this == other); }

    CharView &operator*() { return parent_.hlist_.at(index_); }

  private:
    iterator(LineView &parent, size_t index) : index_(index), parent_(parent) {}

    size_t index_;
    LineView &parent_;
  };

  emcc::iterator_range<iterator> GetSegment(size_t seg) {
    if (seg >= segment_index_.size())
      return emcc::make_range(iterator(*this, 0), iterator(*this, 0));
    if (seg == segment_index_.size() - 1)
      return emcc::make_range(iterator(*this, segment_index_[seg]),
                              iterator(*this, hlist_.size()));
    return emcc::make_range(iterator(*this, segment_index_[seg]),
                            iterator(*this, segment_index_[seg + 1]));
  }

private:
  CharView CreateCharView(size_t point, wchar_t wch) {
    CharView cv;
    cv.point = point;
    cv.character = wch;
    cv.width = wchar_width(wch);
    return cv;
  }

  void ReCompute(size_t seg);

  size_t width_;
  MonoBuffer *parent_;
  // Horizontal list, concept borrowed from TeX.
  std::vector<CharView> hlist_;
  std::vector<size_t> segment_index_;
};

} // namespace emcc::editor
