#include "tui/view.h"

#include <ctype.h>

namespace emcc::tui {

void Page::Reload(size_t start_line) {
  Reset();
  Cursor pos(0, 0);
  size_t start_offset;
  buffer_->ComputeOffset(start_line, 0, start_offset);
  for (size_t i = start_offset;
       i < buffer_->CountChars() && !Cursor::IsBeyond(pos, GetBoundary());
       ++i) {
    size_t w;
    char c;
    std::tie(c, w) = FillPixelAt(pos, i);
    if (w == 0)
      break;
    if (c == MonoBuffer::kNewLine) {
      ++pos.y;
      pos.x = 0;
    } else {
      pos = JumpTo(width(), pos, w);
    }
  }
  // Draw status line.
  std::string content = fmt::format("---- {} of {} ({}, {}) ", start_offset,
                                    buffer_->CountChars(), start_line, 0);
  auto &status_line = framebuffer_->frame_[height() - 1];
  for (size_t i = 0; i < status_line.size(); ++i) {
    if (i < content.size()) {
      status_line[i].shade.character = content[i];
    } else {
      status_line[i].shade.character = '-';
    }
  }
}

std::tuple<char, size_t> Page::FillPixelAt(Cursor at, size_t offset) {
  Cursor boundary(GetBoundary());
  if (Cursor::IsBeyond(at, boundary))
    return std::make_tuple(0, 0);
  Pixel &pixel = framebuffer_->Get(at.y, at.x);
  char c;
  if (!buffer_->Get(offset, c)) {
    return std::make_tuple(0, 0);
  }
  Pixel new_pixel = Pixel::MakeHeadPixel(offset, 1);
  pixel.position = new_pixel.position;
  pixel.shade.character = c;
  return std::make_tuple(c, pixel.length());
}

} // namespace emcc::tui
