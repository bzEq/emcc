// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "edit/mono_buffer.h"
#include "tui/cursor.h"
#include "tui/pixel.h"

namespace emcc::tui {

class BufferView {
public:
  BufferView(MonoBuffer *buffer)
      : height_(24), width_(80), framebuffer_(), buffer_(buffer), baseline_(0),
        cursor_() {}
  void set_baseline(size_t baseline) {
    baseline_ = std::min(baseline, buffer_->CountLines() - 1);
  }
  size_t baseline() const { return baseline_; }
  void set_width(size_t width) { width_ = width; }
  size_t width() const { return width_; }
  void set_height(size_t height) const { height_ = height; }
  size_t height() const { return height_; }
  void FillFramebuffer(size_t num_buffer_line);
  const Pixel &GetPixel(size_t y, size_t x) const { return framebuffer_[y][x]; }
  bool GetPixel(size_t y, size_t x, Pixel &pixel);
  bool GetPixel(Cursor c, Pixel &pixel);
  Cursor GetBoundary() const { return Cursor(framebuffer_.size(), width()); }
  void UpdateStatusLine();
  Cursor cursor() const { return cursor_; }
  void Resize(size_t height, size_t width) {
    set_height(height);
    set_width(width);
    FillFramebuffer();
    UpdateStatusLine();
  }
  // TODO: Make render policy more precisely.
  std::tuple<Cursor, Cursor> GetRenderRange() const {
    return {{0, 0}, GetBoundary()};
  }

  void MoveUp();
  void MoveDown();
  void MoveLeft();
  void MoveRight();

private:
  using FramebufferTy = std::vector<std::vector<Pixel>>;
  bool FindPoint(size_t point, Cursor &c);
  void UpdateFrameBuffer(size_t start_point, size_t len, Cursor start_cursor);
  void ResetFrameBuffer(FramebufferTy &fb, Cursor begin, Cursor end);
  void ResetFrameBuffer(Cursor begin, Cursor end);
  void RewriteFrameBuffer(FramebufferTy &fb, size_t start_point, size_t len,
                          Cursor at, Cursor boundary);
  void RewriteFrameBuffer(size_t start_point, size_t len, Cursor boundary);
  std::tuple<int, size_t> GetCharAndWidth(char c);
  size_t ScrollUp(size_t);
  bool ScrollUp();
  bool ScrollDown();
  bool GetPixel(const FramebufferTy &fb, Cursor at, Pixel &px);
  bool GetPixel(const FramebufferTy &fb, size_t y, size_t x, Pixel &px);

  size_t height_, width_;
  FramebufferTy framebuffer_;
  MonoBuffer *buffer_;
  size_t baseline_;
  Cursor cursor_, render_begin, render_end;
};

}; // namespace emcc::tui
