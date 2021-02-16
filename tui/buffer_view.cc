#include "tui/buffer_view.h"

namespace emcc::tui {

bool BufferView::FindPoint(size_t point, Cursor &c) {
  for (size_t y = 0; y < framebuffer_.size(); ++y) {
    for (size_t x = 0; x < framebuffer_[y].size(); ++x) {
      if (framebuffer_[y][x].position.point == point) {
        c.y = y;
        c.x = x;
        return true;
      }
    }
  }
  return false;
}

bool BufferView::GetPixel(Cursor c, Pixel &pixel) {
  return GetPixel(c.y, c.x, pixel);
}

bool BufferView::GetPixel(size_t y, size_t x, Pixel &pixel) {
  if (y >= framebuffer_.size() || x >= width())
    return false;
  pixel = framebuffer_[y][x];
  return true;
}

void BufferView::Reset(size_t h) {
  if (h > framebuffer_.size()) {
    framebuffer_.resize(h);
  }
  for (size_t i = 0; i < framebuffer_.size(); ++i) {
    framebuffer_[i].resize(width());
    std::fill(framebuffer_[i].begin(), framebuffer_[i].end(), Pixel());
  }
}

void BufferView::FillFramebuffer(size_t nr_buffer_line) {
  Reset(nr_buffer_line);
  Pixel cursor_pixel;
  GetPixel(cursor_, cursor_pixel);
  size_t i;
  buffer_->ComputePoint(baseline_, 0, i);
  Cursor c;
  for (; c.y < nr_buffer_line && i < buffer_->CountChars(); ++i) {
    char ch;
    size_t w;
    std::tie(ch, w) = FillPixelAt(c, i);
    if (w == 0)
      break;
    if (ch == MonoBuffer::kNewLine) {
      ++c.y;
      c.x = 0;
    } else {
      c = Cursor::Goto(width(), c, w);
    }
  }
  if (cursor_pixel.position.point != ~0) {
    if (!FindPoint(cursor_pixel.position.point, cursor_))
      cursor_ = {0, 0};
  }
  // TODO: If framebuffer_ is too large, we need to shrink it.
}

// TODO: Handle '\t' and etc.
static std::tuple<int, size_t> GetCharAndWidth(char c) { return {(int)c, 1}; }

std::tuple<char, size_t> BufferView::FillPixelAt(Cursor at, size_t point) {
  char c;
  if (!buffer_->Get(point, c))
    return {0, 0};
  int print_character;
  size_t w;
  std::tie(print_character, w) = GetCharAndWidth(c);
  for (size_t i = 0; i < w; ++i) {
    if (at.y >= framebuffer_.size())
      break;
    assert(at.x < width());
    Pixel &pixel = framebuffer_[at.y][at.x];
    pixel.shade.character = print_character;
    pixel.position.point = point;
    pixel.set_offset(w, i);
    at = Cursor::Goto(width(), at, 1);
  }
  return {c, w};
}

void BufferView::ScaleFramebuffer(size_t size) {
  if (framebuffer_.size() >= size)
    return;
  size_t i = framebuffer_.size();
  framebuffer_.resize(size);
  for (; i < framebuffer_.size(); ++i)
    framebuffer_[i].resize(width());
}

} // namespace emcc::tui
