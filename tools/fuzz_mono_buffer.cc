#include "edit/mono_buffer.h"

#include <assert.h>
#include <stdio.h>

int main() {
  using namespace emcc;
  char cmd;
  size_t l, c, len;
  MonoBuffer buffer;
  while (~scanf("%c", &cmd) != 0) {
    switch (cmd) {
    case 'i':
      scanf("%zu %zu %zu", &l, &c, &len);
      printf("i %zu %zu %zu\n", l, c, len);
      for (size_t i = 0; i < len; ++i)
        buffer.Insert(l, c, '0');
      break;
    case 'a': {
      scanf("%zu", &len);
      printf("a %zu\n", len);
      std::string s('0', len);
      buffer.Append(s.data(), s.size());
      break;
    }
    case 'e':
      scanf("%zu %zu %zu", &l, &c, &len);
      printf("e %zu %zu %zu\n", l, c, len);
      buffer.Erase(l, c, len);
      break;
    case 'n':
      scanf("%zu %zu", &l, &c);
      printf("n %zu %zu\n", l, c);
      buffer.Insert(l, c, MonoBuffer::kNewLine);
      break;
    default:
      break;
    }
    assert(buffer.Verify());
  }
  return 0;
}
