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
  using Line = Rope<char, std::basic_string>;
  using LineSpan = Rope<Line *, std::vector>;

  LineSpan buffer_;
  size_t size_;

  LineBuffer &InsertEmptyLine(size_t i) {
    auto line = new Line();
    line->Append(kNewLine);
    buffer_.Insert(i, line);
    return *this;
  }

  LineBuffer &InsertLine(size_t i, Line &&line) {
    buffer_.Insert(i, new Line(std::move(line)));
    return *this;
  }

  LineBuffer &RemoveLine(size_t i) {
    if (i >= buffer_.size())
      return *this;
    auto line = buffer_.At(i);
    delete line;
    buffer_.Erase(i, 1);
    return *this;
  }

public:
  static constexpr char kNewLine = '\n';

  static std::unique_ptr<LineBuffer>
  CreateFromFile(const std::string &filename);

  size_t CountLines() {
    size_t s = buffer_.size();
    if (s == 0)
      return 0;
    if (buffer_.At(s - 1)->empty())
      return s - 1;
    return s;
  }

  size_t size() const { return size_; }

  size_t GetLine(size_t line, size_t limit, std::string &content);

  LineBuffer &Insert(size_t line, size_t column, char c);

  LineBuffer &Append(size_t line, char c) { return Insert(line, ~0, c); }

  LineBuffer &Append(char c) { return Insert(~0, ~0, c); }

  size_t Erase(size_t line, size_t column, size_t len);

  bool Verify() {
    for (size_t i = 0; i < buffer_.size(); ++i) {
      auto line = buffer_.At(i);
      for (size_t j = 0; j < line->size(); ++j) {
        if (j != line->size() - 1 && line->At(j) == kNewLine)
          return false;
        if (j == line->size() - 1 && i != buffer_.size() - 1 &&
            line->At(j) != kNewLine)
          return false;
      }
    }
    return true;
  }
};

} // namespace emcc
