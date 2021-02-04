#include "editor/line_buffer.h"
#include "support/random.h"

#include <cassert>
#include <ctime>

int main() {
  using namespace emcc;
  Random rnd(time(nullptr));
  LineBuffer buffer;
  const size_t kMaxLines = 1 << 20;
  const size_t kMaxColumns = 1 << 16;
  auto DoInsert = [&]() {
    size_t line = std::min(buffer.CountLines(), size_t(rnd.Next() * kMaxLines));
    size_t column = 0;
    size_t len = rnd.Next() * kMaxColumns;
    for (size_t i = 0; i < len; ++i) {
      if (rnd.Next() < 1.0 / len)
        buffer.Insert(line, column, LineBuffer::kNewLine);
      else
        buffer.Insert(line, column, '0');
    }
  };
  auto DoAppend = [&]() {
    size_t len = rnd.Next() * kMaxColumns;
    for (size_t i = 0; i < len; ++i) {
      if (rnd.Next() < 1.0 / len)
        buffer.Append(LineBuffer::kNewLine);
      else
        buffer.Append('0');
    }
  };
  auto DoErase = [&]() {
    size_t line =
        std::min(buffer.CountLines() - 1, size_t(rnd.Next() * kMaxLines));
    size_t column = 0;
    size_t len = rnd.Next() * kMaxColumns;
    buffer.Erase(line, column, len);
  };
  size_t it = 0;
  while (true) {
    printf("Iter #%zu: lines: %zu\n", it++, buffer.CountLines());
    auto dice = rnd.Next();
    if (dice < 0.6)
      DoErase();
    else if (dice < 0.8)
      DoInsert();
    else
      DoAppend();
    assert(buffer.Verify());
  }
  return 0;
}
