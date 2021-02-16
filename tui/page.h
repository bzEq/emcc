#pragma once

#include "edit/mono_buffer.h"
#include "support/dynamic_array.h"
#include "support/prefix_sum.h"
#include "tui/cursor.h"

#include <assert.h>
#include <iostream>

namespace emcc::tui {

// Graphical representation of a charater, also offers map to character's
// logical position.
struct Pixel {
  struct {
    int character;
  } shade;

  struct {
    uint8_t head_tail_pair; // A character might span multiple points.
    size_t point;           // Offset at text buffer.
  } position;

  Pixel() { Reset(); }

  void Reset() {
    shade.character = ' ';
    memset(&position, -1, sizeof(position));
  }

  bool operator==(const Pixel &other) {
    return memcmp(this, &other, sizeof(*this)) == 0;
  }

  Pixel &operator=(const Pixel &other) {
    memcpy(this, &other, sizeof(*this));
    return *this;
  }

  bool is_same_position(const Pixel &other) const {
    return memcmp(&this->position, &other.position, sizeof(this->position)) ==
           0;
  }

  bool is_same_shade(const Pixel &other) const {
    return memcmp(&this->shade, &other.shade, sizeof(this->shade)) == 0;
  }

  bool is_head() const {
    return position.head_tail_pair >> (sizeof(position.head_tail_pair) * 8 - 1);
  }
  size_t offset() const {
    return position.head_tail_pair &
           (static_cast<decltype(position.head_tail_pair)>(~0U) >> 1);
  }
  size_t length() const {
    assert(is_head());
    return offset();
  }
  static Pixel MakeHeadPixel(size_t point, size_t len) {
    Pixel p;
    assert(len < (1 << (sizeof(p.position.head_tail_pair) * 8 - 1)));
    p.position.head_tail_pair =
        (1 << (sizeof(p.position.head_tail_pair) * 8 - 1)) | len;
    p.position.point = point;
    return p;
  }
  static Pixel MakeTailPixel(size_t point, size_t offset) {
    Pixel p;
    assert(offset > 0 &&
           offset < (1 << (sizeof(p.position.head_tail_pair) * 8 - 1)));
    p.position.head_tail_pair = offset;
    p.position.point = point;
    return p;
  }
  static std::vector<Pixel> MakeSeries(size_t offset, size_t len) {
    assert(len > 0);
    std::vector<Pixel> ret;
    ret.emplace_back(MakeHeadPixel(offset, len));
    for (size_t i = 1; i < len; ++i)
      ret.emplace_back(MakeTailPixel(offset, i));
    return ret;
  }
};

// Graphical representation of a page.
class Framebuffer {
public:
  friend class Page;
  Framebuffer(size_t width, size_t height) : width_(width), height_(height) {
    Reset();
  }

  void Resize(size_t width, size_t height) {
    width_ = width;
    height_ = height;
  }

  size_t width() const { return width_; }

  size_t height() const { return height_; }

  void Reset() {
    frame_.clear();
    frame_.resize(height_);
    for (size_t i = 0; i < frame_.size(); ++i)
      frame_[i].resize(width_);
  }

  const Pixel &Get(size_t y, size_t x) const { return frame_[y][x]; }

  Pixel &Get(size_t y, size_t x) { return frame_[y][x]; }

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

class Page {
public:
  static constexpr size_t kTabWidth = 2;
  Page(MonoBuffer *buffer, Framebuffer *framebuffer, size_t width,
       size_t height)
      : buffer_(buffer), framebuffer_(framebuffer) {
    Reset();
    framebuffer_->Resize(width, height);
  }
  void Resize(size_t width, size_t height) {
    framebuffer_->Resize(width, height);
  }
  void Reset() { framebuffer_->Reset(); }
  void set_baseline(size_t baseline) { baseline_ = baseline; }
  size_t baseline() const { return baseline_; }
  void Reload();
  size_t width() const { return framebuffer_->width(); }
  size_t height() const { return framebuffer_->height(); }
  Cursor GetBoundary() const { return framebuffer_->GetBoundary(); }
  const Framebuffer &framebuffer() const { return *framebuffer_; }
  Framebuffer &framebuffer() { return *framebuffer_; }
  bool GetPixel(Cursor pos, Pixel &result);
  void UpdateStatusLine(Cursor pos);

private:
  std::tuple<char, size_t> FillPixelAt(Cursor at, size_t offset);

  size_t baseline_;
  MonoBuffer *buffer_;
  Framebuffer *framebuffer_;
};

} // namespace emcc::tui
