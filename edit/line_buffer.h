// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/prefix_sum.h"
#include "support/rope.h"

#include <memory>
#include <string>
#include <vector>

namespace emcc {

// All lines must end with kNewLine except the last one.
class LineBuffer {
public:
  static constexpr char kNewLine = '\n';
  static std::unique_ptr<LineBuffer>
  CreateFromFile(const std::string &filename);
  LineBuffer() {}
  size_t CountLines();
  size_t GetAccumulateChars(size_t line) {
    return accumulate_size_.GetPrefixSum(line);
  }
  size_t CountChars();
  bool Get(size_t line, size_t col, char &c);
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
  bool ComputePosition(size_t offset, size_t &line, size_t &col);
  bool ComputeOffset(size_t line, size_t col, size_t &offset);

private:
  using Line = Rope<char, std::basic_string, 1UL << 13>;
  using LineSpan = Rope<Line *, std::basic_string, 1UL << 10>;

  LineBuffer &InsertEmptyLine(size_t i);
  LineBuffer &InsertLine(size_t i, Line &&line);
  LineBuffer &RemoveLine(size_t i);

  LineSpan buffer_;
  PrefixSum<long> accumulate_size_;
};

} // namespace emcc
