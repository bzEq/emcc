#pragma once

#include "editor/line_buffer.h"
#include "support/dynamic_array.h"
#include "support/prefix_sum.h"

#include <assert.h>
#include <iostream>

namespace emcc::tui {

struct Cursor {
  int x, y;
  Cursor() : x(), y() {}
  Cursor(int x, int y) : x(x), y(y) {}
  bool operator<(const Cursor &other) const {
    if (y < other.y)
      return true;
    if (y == other.y)
      return x < other.x;
    return false;
  }
  bool operator==(const Cursor &other) const {
    return x == other.x && y == other.y;
  }
  bool operator>(const Cursor &other) const {
    if (y > other.y)
      return true;
    if (y == other.y)
      return x > other.x;
    return false;
  }
};

inline int WrapDistance(const int wrap_width, Cursor a, Cursor b) {
  assert(wrap_width > 0);
  assert(a.x < wrap_width && b.x < wrap_width);
  return (b.y - a.y) * wrap_width + b.x - a.x;
}

inline Cursor JumpTo(const int wrap_width, Cursor origin, int distance) {
  assert(wrap_width > 0);
  int dx = distance % wrap_width;
  int dy = distance / wrap_width;
  if (origin.x + dx < 0) {
    dx += wrap_width;
    dy -= 1;
  }
  return Cursor{
      origin.x + dx,
      origin.y + dy,
  };
}

struct LogicalLocation {
  size_t line, col;
  LogicalLocation() : line(~0), col(~0) {}
};

class Viewport {
public:
  Viewport(int width, int height) : width_(width), height_(height) { Reset(); }

  void Reset() {
    locator_.clear();
    locator_.resize(height_);
    for (size_t i = 0; i < height_; ++i)
      locator_[i].resize(width_);
  }

  int width() const { return width_; }

  int height() const { return height_; }

private:
  const int width_, height_;
  // Mapping cursor to logical location in LineBuffer.
  std::vector<std::vector<LogicalLocation>> locator_;
};

class WYSIWYGEditor {
public:
  WYSIWYGEditor(Viewport *view, LineBuffer *buffer)
      : view_(view), x_(0), y_(0), buffer_(buffer) {}

  WYSIWYGEditor &ChangeView(Viewport *view) {
    view_ = view;
    return *this;
  }

  WYSIWYGEditor &ReloadView(size_t start_line);

  WYSIWYGEditor &Move(int y, int x) {
    assert(x >= 0 && y >= 0);
    y_ = std::min(y, view_->height() - 1);
    x_ = std::min(x, view_->width() - 1);
    return *this;
  }

  // Write c at (y, x).
  // TODO: Support character occupying multiple boxes.
  WYSIWYGEditor &Write(int c);

  WYSIWYGEditor &Insert(int c);

  WYSIWYGEditor &Delete();

  WYSIWYGEditor &Backspace();

private:
  Viewport *view_;
  int x_, y_;
  LineBuffer *buffer_;
};

} // namespace emcc::tui
