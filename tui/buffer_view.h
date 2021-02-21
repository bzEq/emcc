// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "edit/mono_buffer.h"
#include "tui/cursor.h"
#include "tui/framebuffer.h"
#include "tui/pixel.h"

namespace emcc::tui {

using FramebufferTy = emcc::tui::Framebuffer;

class BufferView {
public:
  BufferView(MonoBuffer *buffer)
      : version_(0), width_(80), framebuffer_(width_), buffer_(buffer),
        cursor_() {}
  size_t width() const { return width_; }
  size_t height() const { return framebuffer_.height(); }
  Cursor cursor() const { return cursor_; }
  void Resize(size_t height, size_t width);
  const Pixel &GetPixel(Cursor c) const { return framebuffer_.GetPixel(c); }
  bool GetStatusLine(std::string &content) const;
  std::string GetStatusLine() const {
    std::string s;
    GetStatusLine(s);
    return s;
  }
  void GotoLine(size_t baseline);

  bool MoveUp();
  bool MoveDown();
  bool MoveLeft();
  bool MoveRight();
  size_t version() const { return version_; }

  Region GetDiffRegion(size_t src, size_t target) const {
    // TODO: Make render policy more precisely.
    return framebuffer_.region();
  }

  void RewriteFrameBuffer(size_t point, size_t len) {
    RewriteFrameBuffer(point, len, framebuffer_, framebuffer_.region());
  }

  bool ScrollUp();
  bool ScrollDown();

private:
  void RewriteFrameBuffer(size_t point, size_t len, Framebuffer &fb,
                          Region region);
  std::tuple<int, size_t> GetCharAndWidth(char c);
  size_t ScrollUp(size_t);

  size_t version_;
  size_t width_;
  FramebufferTy framebuffer_;
  MonoBuffer *buffer_;
  Cursor cursor_, render_begin, render_end;
};

}; // namespace emcc::tui
