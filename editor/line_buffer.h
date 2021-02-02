// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/rope.h"

#include <memory>
#include <string>
#include <vector>

namespace emcc {

class LineBuffer {
private:
  static constexpr char kNewLine = '\n';
  using Line = Rope<char, std::basic_string>;
  using LineSpan = Rope<Line *, std::vector>;

  LineSpan buffer_;

  void InsertEmptyLine(size_t i) { buffer_.Insert(i, new Line()); }

  void InsertLine(size_t i, Line &&line) {
    buffer_.Insert(i, new Line(std::move(line)));
  }

public:
  static std::unique_ptr<LineBuffer>
  CreateFromFile(const std::string &filename);

  LineBuffer() {}

  LineBuffer &swap(LineBuffer &&other) {
    buffer_.swap(std::move(other.buffer_));
    return *this;
  }

  ~LineBuffer() {
    for (size_t i = 0; i < buffer_.size(); ++i)
      delete buffer_.At(i);
  }

  size_t CountLines() {
    size_t s = buffer_.size();
    if (buffer_.At(s - 1)->empty())
      return s - 1;
    return s;
  }

  size_t GetLine(size_t line, size_t limit, std::string &content);
  bool Insert(size_t line, size_t column, char c);
  size_t Erase(size_t line, size_t column, size_t len, LineBuffer &erased);
  bool Append(size_t line, char c);
  bool Append(char c);
};

} // namespace emcc
