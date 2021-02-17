// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "edit/mono_buffer.h"
#include "tui/cursor.h"
#include "tui/pixel.h"

namespace emcc::tui {

class BufferView {
public:
  BufferView(MonoBuffer *buffer)
      : width_(80), framebuffer_(), buffer_(buffer), baseline_(0), cursor_() {}
  void set_baseline(size_t baseline) {
    baseline_ = std::min(baseline, buffer_->CountLines() - 1);
  }
  size_t baseline() const { return baseline_; }
  void set_width(size_t width) { width_ = width; }
  size_t width() const { return width_; }
  size_t height() const { return framebuffer_.size(); }
  void FillFramebuffer(size_t nr_buffer_line);
  const Pixel &GetPixel(size_t y, size_t x) const { return framebuffer_[y][x]; }
  bool GetPixel(size_t y, size_t x, Pixel &pixel);
  bool GetPixel(Cursor c, Pixel &pixel);
  Cursor GetBoundary() const { return Cursor(framebuffer_.size(), width()); }
  void DrawStatusLine();
  Cursor cursor() const { return cursor_; }

  void MoveUp();
  void MoveDown();
  void MoveLeft();
  void MoveRight();

private:
  void Reset(size_t height);
  bool FindPoint(size_t point, Cursor &c);
  std::tuple<char, size_t> FillPixelAt(Cursor, size_t);
  void ScaleFramebuffer(size_t);

  size_t width_;
  std::vector<std::vector<Pixel>> framebuffer_;
  MonoBuffer *buffer_;
  size_t baseline_;
  Cursor cursor_;
};

}; // namespace emcc::tui
