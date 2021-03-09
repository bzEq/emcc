// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "editor/mono_buffer.h"
#include "support/sys.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 2)
    emcc::Die("Usage %s: <filename>", argv[0]);
  emcc::MMapFile file(argv[1], sysconf(_SC_PAGE_SIZE));
  if (!file.is_open())
    emcc::Die("Can't open %s", argv[1]);
  emcc::editor::MonoBuffer mb;
  for (auto block : file) {
    mb.Append(block.data, block.size());
  }
  std::cerr << mb.CountLines() << " " << mb.CountChars() << std::endl;
  for (size_t i = 0; i < mb.CountChars(); ++i) {
    char c;
    mb.Get(i, c);
    std::cout << c;
  }
  return 0;
}
