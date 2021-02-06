// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "editor/line_buffer.h"
#include "support/sys.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 2)
    emcc::Die("Usage %s: <filename>", argv[0]);
  emcc::MMapFile file(argv[1], sysconf(_SC_PAGE_SIZE));
  if (!file.is_open())
    emcc::Die("Can't open %s", argv[1]);
  for (auto block : file)
    std::cout << std::string(block.data, block.length);
  return 0;
}
