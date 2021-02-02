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

bool LineBuffer::Append(char c) { return Append(buffer_.size() - 1, c); }

bool LineBuffer::Append(size_t l, char c) { return Insert(l, ~0, c); }

bool LineBuffer::Insert(size_t l, size_t c, char x) {
  if (buffer_.size() == 0)
    InsertEmptyLine(0);
  assert(buffer_.size() > 0);
  l = std::min(l, buffer_.size() - 1);
  auto line = buffer_.At(l);
  c = std::min(line->size(), c);
  line->Insert(c, x);
  if (x == kNewLine) {
    auto new_line = line->Split(c + 1);
    assert(new_line.empty() || new_line.At(new_line.size() - 1) == kNewLine);
    InsertLine(l + 1, std::move(new_line));
  }
  return true;
}

size_t LineBuffer::Erase(size_t l, size_t c, size_t len, LineBuffer &erased) {
  if (l >= buffer_.size())
    return 0;
  auto line = buffer_.At(l);
  if (c >= line->size())
    return 0;
  size_t tail = line->size() - c;
  erased.swap(LineBuffer());
  assert(erased.buffer_.size() == 0);
  if (len >= tail && l + 1 < buffer_.size()) {
    assert(line->At(line->size() - 1) == kNewLine);
    buffer_.Erase(l + 1, 1, &erased.buffer_);
    assert(erased.buffer_.size() == 1);
    auto next_line = erased.buffer_.At(0);
    line->Concat(std::move(*next_line));
    line->Erase(c, tail, next_line);
    return tail;
  }
  erased.InsertEmptyLine(0);
  return line->Erase(c, std::min(len, tail), erased.buffer_.At(0));
}

} // namespace emcc
