#pragma once

#include "support/dynamic_array.h"
#include "support/misc.h"

#include <assert.h>
#include <iostream>
#include <memory>
#include <vector>

namespace emcc::layout {

template <typename HBox>
using HListTy = emcc::DynamicArray<HBox>;

template <typename HBox>
class Paragraph {
public:
  class iterator;
  friend class iterator;

  Paragraph(size_t width) : width_(width) {}

  Paragraph(size_t width, HListTy<HBox> &&hlist)
      : width_(width), hlist_(std::move(hlist)) {
    ReCompute();
  }

  size_t height() const { return line_index_.size(); }
  void Resize(size_t width) {
    width_ = width;
    ReCompute();
  }

  template <typename... Args>
  void Insert(int y, int x, Args &&...args);
  template <typename... Args>
  void Append(HBox &&hbox, Args &&...args);
  bool Update(int y, int x, size_t box_len);
  size_t Erase(int y, int x, size_t num_boxes);
  size_t NumBoxes(size_t i) {
    assert(i < line_index_.size());
    if (i == line_index_.size() - 1)
      return hlist_.size() - line_index_[i];
    return line_index_[i + 1] - line_index_[i];
  }

  class iterator {
  public:
    friend class Paragraph;
    iterator &operator++() {
      ++index_;
      return *this;
    }
    bool operator==(const iterator &other) const {
      return &parent_ == &other.parent_ && index_ == other.index_;
    }
    bool operator!=(const iterator &other) const { return !(*this == other); }

    HBox &operator*() { return parent_->hlist_[index_]; }

  private:
    iterator(Paragraph &parent, size_t index)
        : index_(index), parent_(parent) {}

    size_t index_;
    Paragraph &parent_;
  };

  emcc::iterator_range<iterator> GetLine(size_t i) {
    if (i >= line_index_.size())
      return emcc::make_range(iterator(*this, 0), iterator(*this, 0));
    if (i == line_index_.size() - 1)
      return emcc::make_range(iterator(*this, line_index_[i]),
                              iterator(*this, hlist_.size()));
    return emcc::make_range(iterator(*this, line_index_[i]),
                            iterator(*this, line_index_[i + 1]));
  }

private:
  void ReCompute() {
    line_index_.clear();
    size_t current_offset = 0;
    for (size_t i = 0; i < hlist_.size(); ++i) {
      auto &hbox = hlist_.At(i);
      size_t hlen = hbox.length();
      size_t s = current_offset + hlen;
      size_t fill_current_line_badness =
          abs_diff(s, width_) + (s > width_ ? hlen : 0);
      size_t fill_next_line_badness = abs_diff(hlen, width_);
      if (fill_current_line_badness < fill_next_line_badness) {
        current_offset = s;
      } else {
        // std::cout << "Break at " << i << std::endl;
        current_offset = hlen;
        line_index_.emplace_back(i);
      }
    }
  }

  size_t width_;
  HListTy<HBox> hlist_;
  std::vector<size_t> line_index_;
};

} // namespace emcc::layout
