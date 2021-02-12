#pragma once

#include "editor/line_buffer.h"
#include "support/dynamic_array.h"
#include "support/prefix_sum.h"

#include <assert.h>
#include <iostream>

namespace emcc::tui {

// Map a character graphical position to logical position.
struct Point {
  uint8_t offset; // A character might span multiple points.
  struct {
    size_t line, col;
  } position; // Position at text buffer.
  Point() : offset(0), position{~0U, ~0U} {}
};

class Page {
public:
  void ScrollUp(size_t);
  void ScrollDown(size_t);

private:
};

// Graphical representation of a point.
struct Pixel {
  int character;
  Pixel() = default;
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
