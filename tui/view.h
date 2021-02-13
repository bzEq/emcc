#pragma once

#include "edit/mono_buffer.h"
#include "support/dynamic_array.h"
#include "support/prefix_sum.h"
#include "tui/basis.h"

#include <assert.h>
#include <iostream>

namespace emcc::tui {

// Map a character graphical position to logical position.
struct Point {
  uint8_t type_offset_pair; // A character might span multiple points.
  union {
    size_t line;
    size_t point; // Offset at text buffer.
  };
  size_t col; // (line, column) at text buffer.
  Point() : type_offset_pair(~0), line(~0U), col(~0U) {}
  void Reset() {
    type_offset_pair = ~0;
    line = ~0U;
    col = ~0U;
  }
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
  bool is_start() const {
    return type_offset_pair >> (sizeof(type_offset_pair) * 8 - 1);
  }
  size_t offset() const {
    return type_offset_pair &
           (static_cast<decltype(type_offset_pair)>(~0U) >> 1);
  }
  size_t length() const {
    assert(is_start());
    return offset();
  }
  static Point MakeStartPoint(size_t line, size_t col, size_t len) {
    Point p;
    assert(len < (1 << (sizeof(p.type_offset_pair) * 8 - 1)));
    p.type_offset_pair = (1 << (sizeof(p.type_offset_pair) * 8 - 1)) | len;
    p.line = line;
    p.col = col;
    return p;
  }
  static Point MakeStartPoint(size_t offset, size_t len) {
    Point p;
    assert(len < (1 << (sizeof(p.type_offset_pair) * 8 - 1)));
    p.type_offset_pair = (1 << (sizeof(p.type_offset_pair) * 8 - 1)) | len;
    p.point = offset;
    return p;
  }
  static Point MakeTailPoint(size_t line, size_t col, size_t offset) {
    Point p;
    assert(offset > 0 && offset < (1 << (sizeof(p.type_offset_pair) * 8 - 1)));
    p.type_offset_pair = offset;
    p.line = line;
    p.col = col;
    return p;
  }
  static std::vector<Point> MakeSeries(size_t line, size_t col, size_t len) {
    assert(len > 0);
    std::vector<Point> ret;
    ret.emplace_back(MakeStartPoint(line, col, len));
    for (size_t i = 1; i < len; ++i)
      ret.emplace_back(MakeTailPoint(line, col, i));
    return ret;
  }
};

class Framebuffer;

class Page {
public:
  static constexpr size_t kTabWidth = 2;
  Page(MonoBuffer *buffer, Framebuffer *framebuffer, size_t width,
       size_t height)
      : buffer_(buffer), framebuffer_(framebuffer), width_(width),
        height_(height) {
    Reset();
  }
  void Reset() {
    page_.resize(height_);
    for (size_t i = 0; i < page_.size(); ++i)
      page_[i].resize(width_);
  }
  const Point &Get(int y, int x) const { return page_[y][x]; }
  void Reload(size_t start_line);
  size_t width() const { return width_; }
  size_t height() const { return height_; }
  bool Erase(Cursor pos);
  size_t WriteTo(size_t line, size_t col, Cursor pos);
  size_t WriteTo(size_t offset, Cursor pos);
  Cursor GetBoundary() const;
  void FillFrame(Cursor begin, Cursor end);
  const Framebuffer &GetFrameBuffer() const { return *framebuffer_; }

private:
  MonoBuffer *buffer_;
  Framebuffer *framebuffer_;
  size_t width_, height_;
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

  Cursor GetBoundary() const { return Cursor(height(), width()); }

private:
  size_t width_, height_;
  std::vector<std::vector<Pixel>> frame_;
};

} // namespace emcc::tui
