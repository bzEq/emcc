#pragma once

#include "tui/cursor.h"
#include "tui/pixel.h"

namespace emcc::tui {

class Framebuffer {
public:
  friend class Page;
  Framebuffer() = default;

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
    assert(y < frame_.size());
    auto &line = frame_[y];
    assert(x < line.size());
    line.emplace(line.begin() + x, std::forward<Args>(args)...);
    return *this;
  }

  Cursor GetBoundary() const { return Cursor(height(), width()); }

private:
  size_t width_, height_;
  std::vector<std::vector<Pixel>> frame_;
};

} // namespace emcc::tui
