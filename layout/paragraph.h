#pragma once

#include "support/dynamic_array.h"
#include "support/misc.h"

#include <assert.h>
#include <iostream>
#include <memory>
#include <vector>

namespace emcc::layout {

// FIXME: Using rope would be cheaper.
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
  void Insert(int y, int x, Args &&...args) {
    if (line_index_.empty()) {
      Append(std::forward<Args>(args)...);
      return;
    }
    y = std::min((size_t)y, line_index_.size() - 1);
    x = std::min((size_t)x, NumBoxes(y));
    size_t i = line_index_[y] + x;
    hlist_.Insert(i, std::forward<Args>(args)...);
    ReCompute(y);
  }

  template <typename... Args>
  void Append(Args &&...args) {
    hlist_.Append(std::forward<Args>(args)...);
    if (line_index_.empty())
      ReCompute();
    else
      ReCompute(line_index_.size() - 1);
  }

  bool Update(int y, int x, size_t box_len);
  size_t Erase(int y, int x, size_t num_boxes);
  size_t NumBoxes(size_t i) {
    if (i >= line_index_.size())
      return 0;
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

    HBox &operator*() { return parent_.hlist_.At(index_); }

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
  void ReCompute(size_t line) {
    line = std::min(line, line_index_.size());
    size_t start_from = 0;
    if (!line_index_.empty())
      start_from = line_index_[line];
    line_index_.resize(line);
    size_t current_offset = 0;
    for (size_t i = start_from; i < hlist_.size(); ++i) {
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

  void ReCompute() { ReCompute(0); }

  size_t width_;
  HListTy<HBox> hlist_;
  std::vector<size_t> line_index_;
};

} // namespace emcc::layout
