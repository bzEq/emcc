#include "edit/line_buffer.h"

#include <assert.h>
#include <stdio.h>

int main() {
  using namespace emcc;
  char cmd;
  size_t l, c, len;
  LineBuffer buffer;
  while (~scanf("%c", &cmd) != 0) {
    switch (cmd) {
    case 'i':
      scanf("%zu %zu %zu", &l, &c, &len);
      printf("i %zu %zu %zu\n", l, c, len);
      for (size_t i = 0; i < len; ++i)
        buffer.Insert(l, c, '0');
      buffer.Insert(l, ~0, '\n');
      break;
    case 'a':
      scanf("%zu", &len);
      printf("a %zu\n", len);
      for (size_t i = 0; i < len; ++i)
        buffer.Append('0');
      buffer.Append('\n');
      break;
    case 'e':
      scanf("%zu %zu %zu", &l, &c, &len);
      printf("e %zu %zu %zu\n", l, c, len);
      buffer.Erase(l, c, len);
      break;
    default:
      break;
    }
    assert(buffer.Verify());
  }
  return 0;
}
