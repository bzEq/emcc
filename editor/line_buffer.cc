#include "editor/line_buffer.h"
#include "support/sys.h"

#include <iostream>

namespace emcc {

size_t LineBuffer::GetLine(size_t l, size_t limit, std::string &content) {
  if (l >= buffer_.size())
    return 0;
  auto line = buffer_.At(l);
  size_t i = 0;
  for (; i < std::min(limit, line->size()); ++i)
    content.push_back(line->At(i));
  return i;
}

LineBuffer &LineBuffer::Insert(size_t l, size_t column, char c) {
  l = std::min(buffer_.size() - 1, l);
  if (l == ~0)
    l = 0;
  for (size_t i = buffer_.size(); i <= l; ++i) {
    if (i == l)
      InsertLine(i, Line());
    else
      InsertEmptyLine(i);
  }
  assert(l < buffer_.size());
  auto line = buffer_.At(l);
  column = std::min(line->size(), column);
  if (column != 0) {
    char prev_char = line->At(column - 1);
    if (prev_char == kNewLine)
      column = column - 1;
  }
  line->Insert(column, c);
  if (c == kNewLine) {
    Line new_line = line->Split(column + 1);
    InsertLine(l + 1, std::move(new_line));
  }
  ++size_;
  return *this;
}

size_t LineBuffer::Erase(size_t l, size_t column, size_t len) {
  size_t count = 0;
  if (l >= buffer_.size())
    return count;
  auto line = buffer_.At(l);
  if (column >= line->size())
    return count;
  assert(~0 - column >= len);
  Line mid = line->Split(column);
  if (len >= mid.size()) {
    if (l + 1 < buffer_.size()) {
      auto next_line = buffer_.At(l + 1);
      line->Concat(std::move(*next_line));
      RemoveLine(l + 1);
    }
    size_ -= mid.size();
    return mid.size();
  }
  Line tail = mid.Split(column + len);
  line->Concat(std::move(tail));
  size_ -= mid.size();
  return mid.size();
}

} // namespace emcc
