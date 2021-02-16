#pragma once

#include "edit/mono_buffer.h"
#include "support/dynamic_array.h"
#include "support/prefix_sum.h"
#include "tui/cursor.h"
#include "tui/framebuffer.h"
#include "tui/pixel.h"

#include <assert.h>
#include <iostream>

namespace emcc::tui {

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
