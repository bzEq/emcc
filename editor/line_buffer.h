// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/rope.h"

#include <memory>
#include <string>
#include <vector>

namespace emcc {

template <typename T>
using Arc = std::shared_ptr<T>;

template <typename T>
using Own = std::unique_ptr<T>;

class LineBuffer {
private:
  using Line = Rope<char, std::basic_string>;
  using BufferTy = Rope<Line *, std::vector>;
  BufferTy buffer_;
  static constexpr char kNewLine = '\n';

  template <typename... Args>
  Line *CreateLine(Args &&...args) {
    return new Line(std::forward<Args>(args)...);
  }

  void ClearBuffer(BufferTy *buffer) {
    for (size_t i = 0; i < buffer->size(); ++i)
      delete buffer->At(i);
  }

public:
  static std::unique_ptr<LineBuffer>
  CreateFromFile(const std::string &filename);

  LineBuffer() { buffer_.Append(CreateLine()); }

  ~LineBuffer() {
    ClearBuffer(&buffer_);
  }

  size_t CountLines() {
    size_t s = buffer_.size();
    if (buffer_.At(s - 1)->empty())
      return s - 1;
    return s;
  }

  size_t GetLine(size_t line, size_t limit, std::string *content);
  bool Insert(size_t line, size_t column, char c);
  size_t Erase(size_t line, size_t column, size_t len);
  bool Append(size_t line, char c);
  bool Append(char c);
};

} // namespace emcc
