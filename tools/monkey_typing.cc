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
    size_t line = rnd.Next() * kMaxLines;
    size_t column = rnd.Next() * kMaxColumns;
    size_t len =
        std::min(kMaxColumns - column, size_t(rnd.Next() * kMaxColumns));
    printf("Inserting to %zu:%zu:%zu", line, column, len);
    std::cout << std::endl;    
    for (size_t i = 0; i < len; ++i) {
      buffer.Insert(line, column, '0');
    }
  };
  auto DoErase = [&]() {
    size_t line = rnd.Next() * kMaxLines;
    size_t column = rnd.Next() * kMaxColumns;
    size_t len =
        std::min(kMaxColumns - column, size_t(rnd.Next() * kMaxColumns));
    printf("Erasing %zu:%zu:%zu", line, column, len);
    std::cout << std::endl;    
    buffer.Erase(line, column, len);
  };
  size_t it = 0;
  while (true) {
    printf("Iter #%zu: lines: %zu", it++, buffer.CountLines());
    std::cout << std::endl;
    if (rnd.Next() < 0.5)
      DoErase();
    else
      DoInsert();
    assert(buffer.Verify());
  }
  return 0;
}
