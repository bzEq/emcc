#pragma once

#include "editor/line_buffer.h"
#include "support/dynamic_array.h"
#include "support/prefix_sum.h"

#include <assert.h>
#include <iostream>

namespace emcc::tui {

// Map a character graphical position to logical position.
struct Point {
  uint8_t offset;   // A character might span multiple points.
  size_t line, col; // Position at text buffer.
  Point() : offset(~0), line(~0U), col(~0U) {}
  bool operator==(const Point &other) {
    return memcmp(this, &other, sizeof(*this)) == 0;
  }
  Point &operator=(const Point &other) {
    memcpy(this, &other, sizeof(*this));
    return *this;
  }
  bool CompareAndUpdate(const Point &other) {
    if (*this == other)
      return false;
    *this = other;
    return true;
  }
};

class Page {
public:
  void ScrollUp(size_t);
  void ScrollDown(size_t);

  const Point &Get(int y, int x) const;

private:
  std::vector<std::vector<Point>> page_;
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

  size_t width() const { return width_; }

  size_t height() const { return height_; }

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

  void FillRange(const Page &page, Cursor begin, Cursor end);

  void FillFull(const Page &page);

private:
  size_t width_, height_;
  std::vector<std::vector<Pixel>> frame_;
};

} // namespace emcc::tui
