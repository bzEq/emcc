#include "tui/view.h"

#include <ctype.h>

namespace emcc::tui {

Cursor Page::GetBoundary() const { return Cursor(height(), width()); }

void Page::Reload(size_t start_line) {
  Reset();
  Cursor pos(0, 0);
  size_t start_offset;
  buffer_->ComputeOffset(start_line, 0, start_offset);
  for (size_t i = start_offset;
       i < buffer_->CountChars() && !Cursor::IsBeyond(pos, GetBoundary());
       ++i) {
    size_t line, col;
    buffer_->ComputePosition(i, line, col);
    size_t n = WriteTo(line, col, pos);
    if (n == 0)
      break;
    char c;
    buffer_->Get(i, c);
    if (c == MonoBuffer::kNewLine) {
      ++pos.y;
      pos.x = 0;
    } else {
      pos = JumpTo(width(), pos, n);
    }
  }
}

bool Page::Erase(Cursor pos) {
  Cursor current = pos;
  Cursor boundary(GetBoundary());
  if (Cursor::IsBeyond(current, boundary))
    return false;
  Point point = page_[current.y][current.x];
  if (!point.is_start()) {
    current = JumpTo(width(), current, -point.offset());
  }
  assert(pos.x >= 0);
  if (pos.y < 0) {
    current.y = current.x = 0;
  }
  for (; !Cursor::IsBeyond(current, JumpTo(width(), pos, 1));
       current = JumpTo(width(), current, 1)) {
    page_[current.y][current.x].Reset();
  }
  return true;
}

size_t Page::WriteTo(size_t line, size_t col, Cursor pos) {
  Cursor boundary(GetBoundary());
  if (Cursor::IsBeyond(pos, boundary))
    return 0;
  Point point = page_[pos.y][pos.x];
  // Avoid writing at the middle of a character.
  if (!point.is_start())
    return 0;
  // Erase this point first.
  if (!Erase(pos))
    return 0;
  Point new_point = Point::MakeStartPoint(line, col, 1);
  if (new_point == point)
    return 0;
  // std::cout << "b: " << new_point.line << " " << new_point.col << std::endl;
  // std::cout << "c: " << pos.y << " " << pos.x << std::endl;
  page_[pos.y][pos.x] = new_point;
  return new_point.length();
}

void Page::FillFrame(Cursor begin, Cursor end) {
  // std::cout << end.y << " " << end.x << std::endl;
  for (Cursor current = begin; !Cursor::IsBeyond(current, end);
       current = JumpTo(width(), current, 1)) {
    if (Cursor::IsBeyond(current, GetBoundary()) ||
        Cursor::IsBeyond(current, framebuffer_->GetBoundary()))
      break;
    Point p = Get(current.y, current.x);
    Pixel pixel;
    char ch;
    if (p.is_start()) {
      if (!buffer_->Get(p.line, p.col, ch)) {
        ch = ' ';
      }
    } else {
      ch = ' ';
    }
    // std::cout << current.y << " " << current.x << " " << (int)ch <<
    // std::endl;
    pixel.character = ch;
    framebuffer_->Set(current.y, current.x, std::move(pixel));
  }
}

} // namespace emcc::tui
