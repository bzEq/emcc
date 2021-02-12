#include "tui/view.h"

#include <ctype.h>

namespace emcc::tui {

void Page::Reload(size_t start_line) {
  Reset();
  Cursor c(0, 0);
  for (size_t i = start_line; i < buffer_->CountLines(); ++i) {
    std::string content;
    buffer_->GetLine(i, ~0, content);
    for (size_t j = 0; j < content.size(); ++j) {
      char c = content[j];
    }
  }
}

bool Page::Erase(Cursor pos) {
  Cursor current = pos;
  Cursor boundary(height(), width());
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

size_t Page::WriteAt(Cursor pos, char c) {
  // Erase content first.
  if (!Erase(pos))
    return 0;
  switch (c) {
  case LineBuffer::kNewLine: {
    break;
  }
  case '\t': {
    break;
  }
  default: {
    break;
  }
  }
  return 1;
}

} // namespace emcc::tui
