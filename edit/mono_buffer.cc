#include "edit/mono_buffer.h"

#include <iostream>

namespace emcc {

bool MonoBuffer::Get(size_t offset, char &c) {
  if (offset >= buffer_.size())
    return false;
  c = buffer_.At(offset);
  return true;
}

MonoBuffer &MonoBuffer::Insert(size_t offset, char c) {
  offset = std::min(buffer_.size(), offset);
  size_t line, col;
  ComputePosition(offset, line, col);
  if (line >= line_size_.size()) {
    assert(line == line_size_.size());
    line_size_.Insert(line, 0);
  }
  buffer_.Insert(offset, c);
  line_size_.Add(line, 1);
  if (c == kNewLine) {
    size_t new_line_size = line_size_.At(line) - (col + 1);
    line_size_.Add(line, -new_line_size);
    line_size_.Insert(line + 1, new_line_size);
  }
  return *this;
}

MonoBuffer &MonoBuffer::Append(char c) { return Insert(buffer_.size(), c); }

void MonoBuffer::ComputePosition(size_t offset, size_t &line, size_t &col) {
  line = line_size_.LowerBound(offset);
  if (line == line_size_.size()) {
    col = 0;
    return;
  }
  if (line == 0)
    col = offset;
  else
    col = offset - line_size_.GetPrefixSum(line - 1);
}

size_t MonoBuffer::CountLines() {
  size_t s = line_size_.size();
  if (s == 0)
    return 0;
  if (line_size_.At(s - 1) == 0)
    return s - 1;
  return s;
}

} // namespace emcc
