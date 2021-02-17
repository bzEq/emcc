#pragma once

#include "support/prefix_sum.h"
#include "support/rope.h"

#include <memory>
#include <string>
#include <vector>

namespace emcc {

class MonoBuffer {
public:
  static constexpr char kNewLine = '\n';
  static constexpr size_t npos = ~0UL;
  static std::unique_ptr<MonoBuffer>
  CreateFromFile(const std::string &filename);
  MonoBuffer() {}
  size_t CountLines();
  size_t CountChars() { return buffer_.size(); }
  bool Get(size_t line, size_t col, char &c);
  bool Get(size_t offset, char &c);
  size_t GetLine(size_t line, size_t limit, std::string &content);
  MonoBuffer &Insert(size_t offset, char c);
  MonoBuffer &Insert(size_t line, size_t column, char c);
  MonoBuffer &Append(size_t line, char c) { return Insert(line, ~0, c); }
  MonoBuffer &Append(const char *data, size_t len);
  MonoBuffer &Append(char c);
  MonoBuffer &Concat(MonoBuffer &&other);
  MonoBuffer Split(size_t offset);
  size_t Erase(size_t offset, size_t len);
  size_t Erase(size_t line, size_t column, size_t len);
  size_t Erase(size_t line, size_t column, size_t len, MonoBuffer &erased);
  bool SaveFile(const std::string &filename);
  void ComputePosition(size_t offset, size_t &line, size_t &col);
  void ComputeOffset(size_t line, size_t col, size_t &offset);
  void ComputePoint(size_t line, size_t col, size_t &point) {
    return ComputeOffset(line, col, point);
  }
  size_t GetLineSize(size_t line) {
    assert(line < line_size_.size());
    return line_size_.At(line);
  }
  bool Verify();
  void set_filename(const std::string &filename) { filename_ = filename; }
  const std::string &filename() const { return filename_; }

private:
  using StorageTy = Rope<char, 1UL << 12>;

  StorageTy buffer_;
  PrefixSum<long> line_size_;
  std::string filename_;
};

} // namespace emcc
