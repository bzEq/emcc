#include "editor/line_buffer.h"
#include "support/sys.h"

namespace emcc {

bool LineBuffer::Append(char c) { return Append(buffer_.size() - 1, c); }

bool LineBuffer::Append(size_t l, char c) {
  if (l >= buffer_.size())
    return false;
  buffer_.At(l)->Append(c);
  if (c == kNewLine)
    buffer_.Insert(l + 1, CreateLine());
  return true;
}

size_t LineBuffer::GetLine(size_t l, size_t limit, std::string *content) {
  if (l >= buffer_.size())
    return 0;
  auto line = buffer_.At(l);
  size_t i = 0;
  for (; i < std::min(limit, line->size()); ++i)
    content->push_back(line->At(i));
  return i;
}

bool LineBuffer::Insert(size_t l, size_t c, char x) {
  if (l >= buffer_.size())
    return false;
  auto line = buffer_.At(l);
  line->Insert(c, x);
  if (x == kNewLine) {
    auto new_line = line->Split(c + 1);
    assert(new_line.empty() || new_line.At(new_line.size() - 1) == kNewLine);
    buffer_.Insert(l + 1, CreateLine(std::move(new_line)));
  }
  return true;
}

size_t LineBuffer::Erase(size_t l, size_t c, size_t len) {
  if (l >= buffer_.size())
    return 0;
  auto line = buffer_.At(l);
  if (c >= line->size())
    return 0;
  size_t tail = line->size() - c;
  if (len >= tail && l + 1 < buffer_.size()) {
    assert(line->At(line->size() - 1) == kNewLine);
    // FIXME: Better management of BufferTy.
    BufferTy erased;
    buffer_.Erase(l + 1, 1, &erased);
    assert(erased.size() == 1);
    auto next_line = erased.At(0);
    line->Erase(c, tail);
    line->Concat(std::move(*next_line));
    ClearBuffer(&erased);
    return tail;
  }
  return line->Erase(c, std::min(len, tail));
}

} // namespace emcc
