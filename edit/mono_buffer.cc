#include "edit/mono_buffer.h"
#include "support/sys.h"

#include <iostream>

namespace emcc {

bool MonoBuffer::Get(size_t offset, char &c) {
  if (offset >= buffer_.size())
    return false;
  c = buffer_.At(offset);
  return true;
}

bool MonoBuffer::Get(size_t line, size_t col, char &c) {
  size_t offset;
  ComputeOffset(line, col, offset);
  return Get(offset, c);
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
  } else {
    col = offset - line_size_.GetPrefixSum(line - 1);
  }
  if (offset != 0 && buffer_.At(offset - 1) == kNewLine) {
    ++line;
    col = 0;
  }
}

void MonoBuffer::ComputeOffset(size_t line, size_t col, size_t &offset) {
  line = std::min(line, line_size_.size());
  col = line < line_size_.size()
            ? std::min(col, static_cast<decltype(col)>(line_size_.At(line)))
            : 0;
  offset = line_size_.GetPrefixSum(line - 1) + col;
}

size_t MonoBuffer::CountLines() {
  size_t s = line_size_.size();
  if (s == 0)
    return 0;
  if (line_size_.At(s - 1) == 0)
    return s - 1;
  return s;
}

std::unique_ptr<MonoBuffer>
MonoBuffer::CreateFromFile(const std::string &filename) {
  MMapFile file(filename, 64UL << 20);
  if (!file.is_open())
    return nullptr;
  auto buffer = std::make_unique<MonoBuffer>();
  for (auto block : file) {
    for (size_t i = 0; i < block.size(); ++i)
      buffer->Append(block.data[i]);
  }
  return buffer;
}

} // namespace emcc
