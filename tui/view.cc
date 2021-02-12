#include "tui/view.h"

#include <ctype.h>

namespace emcc::tui {

Cursor Page::GetBoundary() const { return Cursor(height(), width()); }

void Page::Reload(size_t start_line) {
  Reset();
  Cursor pos(0, 0);
  for (size_t i = start_line; i < buffer_->CountLines() && pos < GetBoundary();
       ++i) {
    size_t n = 0;
    std::string content;
    buffer_->GetLine(i, ~0, content);
    for (size_t j = 0; j < content.size() && pos < GetBoundary(); ++j) {
      n = WriteTo(i, j, pos);
      if (n == 0)
        break;
      if (content[j] == LineBuffer::kNewLine) {
        pos.y++;
        pos.x = 0;
      } else {
        pos = JumpTo(width(), pos, n);
      }
    }
    if (n == 0)
      break;
  }
}

bool Page::Erase(Cursor pos) {
  Cursor current = pos;
  Cursor boundary(GetBoundary());
  if (current >= boundary)
    return false;
  Point point = page_[current.y][current.x];
  if (!point.is_start()) {
    current = JumpTo(width(), current, -point.offset());
  }
  assert(pos.x >= 0);
  if (pos.y < 0) {
    current.y = current.x = 0;
  }
  for (; current <= pos; current = JumpTo(width(), current, 1)) {
    page_[current.y][current.x].Reset();
  }
  return true;
}

size_t Page::WriteTo(size_t line, size_t col, Cursor pos) {
  Cursor boundary(GetBoundary());
  if (pos >= boundary || pos.y < 0)
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
  page_[pos.y][pos.x] = std::move(new_point);
  return 1;
}

} // namespace emcc::tui
