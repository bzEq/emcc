#pragma once

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

// Used to map char's logical index to view cursor.
class LineShape {
public:
  LineShape(int width) : width_(width) {}

  int GetWidth(size_t i) { return index_.At(i); }

  int width() const { return width_; }

  LineShape &Insert(size_t i, int width) {
    assert(width > 0);
    index_.Insert(i, width);
    return *this;
  }

  // Given offset find index.
  size_t FindIndex(int distance) { return index_.UpperBound(distance); }

  size_t size() const { return index_.size(); }

private:
  const int width_;
  PrefixSum<int> index_;
};

class Viewport {
public:
  Viewport(int width) : width_(width), start_line_(0) {}

  bool GetLineColumn(Cursor cursor, size_t &line, size_t &col) {
    size_t index = SearchLineShape(cursor);
    if (index == ~0)
      return false;
    line = start_line_ + index;
    Cursor start = GetStartCursor(index);
    LineShape &ls = view_.At(index);
    int distance = WrapDistance(width_, start, cursor);
    col = ls.FindIndex(distance);
    if (col == ~0)
      return false;
    return true;
  }

private:
  const int width_;
  size_t start_line_;
  DynamicArray<LineShape> view_;
  PrefixSum<int> line_span_;
};

} // namespace emcc::tui
