#pragma once

#include "editor/line_buffer.h"
#include "support/dynamic_array.h"
#include "support/prefix_sum.h"

#include <assert.h>
#include <iostream>
#include <ncurses.h>

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

// Map a character graphical position to logical position.
struct Point {
  uint8_t offset; // A character might span multiple points.
  struct {
    size_t line, col;
  } position; // Position at text buffer.
  Point() : offset(0), position{~0, ~0} {}
};

class Page {};

// Graphical representation of a point.
struct Pixel {
  int character;
  Pixel(int c) : character(c) {}
};

// Graphical representation of a page.
class Framebuffer {
public:
  Framebuffer(size_t width, size_t height) : width_(width), height_(height) {
    Reset();
  }

  void Reset() {
    frame_.resize(height_);
    for (size_t i = 0; i < frame_.size(); ++i)
      frame_[i].resize(width_);
  }

  const Pixel &Get(size_t y, size_t x) const { return frame_[y][x]; }

  template <typename... Args>
  Framebuffer &Set(size_t y, size_t x, Args &&...args) {
    auto &line = frame_[y];
    line.emplace(line.begin() + x, std::forward<Args>(args)...);
    return *this;
  }

private:
  size_t width_, height_;
  std::vector<std::vector<Pixel>> frame_;
};

} // namespace emcc::tui
