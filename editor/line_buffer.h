// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/rope.h"

#include <memory>
#include <string>
#include <vector>

namespace emcc {

// All lines must end with kNewLine except the last one.
class LineBuffer {
private:
  using Line = Rope<char, std::basic_string, 1UL << 13>;
  using LineSpan = Rope<Line *, std::basic_string, 1UL << 10>;

  LineSpan buffer_;
  size_t size_;

  LineBuffer &InsertEmptyLine(size_t i);
  LineBuffer &InsertLine(size_t i, Line &&line);
  LineBuffer &RemoveLine(size_t i);

public:
  static constexpr char kNewLine = '\n';
  static std::unique_ptr<LineBuffer>
  CreateFromFile(const std::string &filename);
  LineBuffer() : size_(0) {}
  size_t CountLines();
  // TODO: Use fenwick tree?
  size_t accumalte_size(size_t line) const;
  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }
  size_t GetLine(size_t line, size_t limit, std::string &content);
  LineBuffer &Insert(size_t line, size_t column, char c);
  LineBuffer &Append(size_t line, char c) { return Insert(line, ~0, c); }
  LineBuffer &Append(char c) { return Insert(~0, ~0, c); }
  LineBuffer &Concat(LineBuffer &&other);
  LineBuffer Split(size_t line);
  size_t Erase(size_t line, size_t column, size_t len);
  size_t Erase(size_t line, size_t column, size_t len, LineBuffer &erased);
  bool Verify();
  bool SaveFile(const std::string &filename);
};

} // namespace emcc
