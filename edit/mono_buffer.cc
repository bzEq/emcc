#include "edit/mono_buffer.h"
#include "support/misc.h"
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

MonoBuffer &MonoBuffer::Insert(size_t line, size_t col, char c) {
  size_t offset;
  ComputeOffset(line, col, offset);
  return Insert(offset, c);
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
    assert(line_size_.At(line) > 0);
    if (new_line_size)
      line_size_.Insert(line + 1, new_line_size);
  }
  return *this;
}

MonoBuffer &MonoBuffer::Append(char c) { return Insert(buffer_.size(), c); }

MonoBuffer &MonoBuffer::Append(const char *data, size_t len) {
  size_t i = 0;
  if (!buffer_.empty() && buffer_.At(buffer_.size() - 1) != kNewLine) {
    for (; i < len; ++i) {
      Append(data[i]);
      if (data[i] == kNewLine) {
        ++i;
        break;
      }
    }
  }
  if (i >= len) {
    return *this;
  }
  assert(buffer_.empty() || buffer_.At(buffer_.size() - 1) == kNewLine);
  buffer_.Append(data + i, len - i);
  size_t current = 0;
  for (; i < len; ++i) {
    ++current;
    if (data[i] == kNewLine) {
      assert(current);
      line_size_.Insert(CountLines(), current);
      current = 0;
    }
  }
  if (current)
    line_size_.Insert(CountLines(), current);
  return *this;
}

void MonoBuffer::ComputePosition(size_t offset, size_t &line, size_t &col) {
  offset = std::min(buffer_.size(), offset);
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

// Any value in line_size_ should not be zero.
size_t MonoBuffer::CountLines() { return line_size_.size(); }

std::unique_ptr<MonoBuffer>
MonoBuffer::CreateFromFile(const std::string &filename) {
  auto membuf = MemoryBuffer::OpenIfExists(filename);
  if (!membuf)
    return nullptr;
  auto buffer = std::make_unique<MonoBuffer>();
  buffer->Append(membuf->buffer(), membuf->length());
  buffer->set_filename(filename);
  return buffer;
}

size_t MonoBuffer::Erase(size_t line, size_t col, size_t len) {
  size_t offset;
  ComputeOffset(line, col, offset);
  return Erase(offset, len);
}

size_t MonoBuffer::Erase(size_t offset, size_t len) {
  if (offset >= buffer_.size())
    return 0;
  len = std::min(buffer_.size() - offset, len);
  size_t begin_line, begin_col;
  ComputePosition(offset, begin_line, begin_col);
  size_t end_line, end_col;
  ComputePosition(offset + len, end_line, end_col);
  for (size_t complete_line = begin_line + 1; complete_line < end_line;
       ++complete_line) {
    line_size_.Remove(begin_line + 1);
  }
  if (begin_line == end_line) {
    line_size_.Add(begin_line, -(end_col - begin_col));
  } else {
    size_t old_current_size = line_size_.At(begin_line);
    line_size_.Add(begin_line, -(old_current_size - begin_col));
    // Now end_line is supposed to be (begin_line + 1).
    if (begin_line + 1 < line_size_.size()) {
      line_size_.Add(begin_line + 1, -end_col);
      line_size_.Add(begin_line, line_size_.At(begin_line + 1));
      line_size_.Remove(begin_line + 1);
    }
  }
  if (line_size_.At(begin_line) == 0) {
    assert(begin_line == line_size_.size() - 1);
    line_size_.Remove(begin_line);
  }
  return buffer_.Erase(offset, len);
}

size_t MonoBuffer::GetLine(size_t line, size_t limit, std::string &content) {
  if (line >= line_size_.size())
    return 0;
  size_t offset;
  ComputeOffset(line, 0, offset);
  size_t i = offset;
  for (; i < offset + std::min(limit, static_cast<decltype(limit)>(
                                          line_size_.At(line)));
       ++i) {
    content.push_back(buffer_.At(i));
  }
  return i - offset;
}

bool MonoBuffer::Verify() {
  std::vector<long> stats;
  long current = 0;
  for (size_t i = 0; i < buffer_.size(); ++i) {
    ++current;
    if (buffer_.At(i) == kNewLine) {
      stats.push_back(current);
      current = 0;
    }
  }
  if (current)
    stats.push_back(current);
  if (abs_diff(stats.size(), line_size_.size()) != 0)
    return false;
  for (size_t i = 0; i < std::min(stats.size(), line_size_.size()); ++i) {
    if (stats[i] != line_size_.At(i))
      return false;
  }
  return true;
}

} // namespace emcc
