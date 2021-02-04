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
    size_t column = rnd.Next() * kMaxColumns;
    size_t len =
        std::min(kMaxColumns - column, size_t(rnd.Next() * kMaxColumns));
    for (size_t i = 0; i < len; ++i) {
      buffer.Insert(line, column, '0');
    }
  };
  auto DoAppend = [&]() {
    if (rnd.Next() < 0.01)
      buffer.Append(LineBuffer::kNewLine);
    else
      buffer.Append('0');
  };
  auto DoErase = [&]() {
    size_t line = rnd.Next() * kMaxLines;
    size_t column = rnd.Next() * kMaxColumns;
    size_t len =
        std::min(kMaxColumns - column, size_t(rnd.Next() * kMaxColumns));
    buffer.Erase(line, column, len);
  };
  size_t it = 0;
  while (true) {
    printf("Iter #%zu: lines: %zu\n", it++, buffer.CountLines());
    auto dice = rnd.Next();
    if (dice < 0.1)
      DoErase();
    else if (dice < 0.3)
      DoInsert();
    else
      DoAppend();
    assert(buffer.Verify());
  }
  return 0;
}
