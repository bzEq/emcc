#include "tui/page.h"

#include <ctype.h>

namespace emcc::tui {

void Page::Reload() {
  Reset();
  Cursor pos(0, 0);
  size_t start_offset;
  buffer_->ComputeOffset(baseline_, 0, start_offset);
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
      pos = Cursor::Goto(width(), pos, w);
    }
  }
}

bool Page::GetPixel(Cursor pos, Pixel &pixel) {
  if (Cursor::IsBeyond(pos, GetBoundary()))
    return false;
  pixel = framebuffer_->Get(pos.y, pos.x);
  return true;
}

void Page::UpdateStatusLine(Cursor pos) {
  Pixel pixel;
  if (!GetPixel(pos, pixel) || pixel.position.point == ~0)
    return;
  size_t line, col;
  buffer_->ComputePosition(pixel.position.point, line, col);
  // Draw status line.
  std::string content = fmt::format(
      "----  {}    Offset: {} of {} Line: {} of {}  ", buffer_->filename(),
      pixel.position.point, buffer_->CountChars(), line, buffer_->CountLines());
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
