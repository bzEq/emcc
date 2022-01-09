// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "core/line_buffer.h"
#include "support/sys.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 2)
    emcc::Die("Usage %s: <filename>", argv[0]);
  std::string filename(argv[1]);
  auto membuf = emcc::MemoryBuffer::OpenIfExists(filename);
  auto data = membuf->buffer();
  size_t len = membuf->length();
  emcc::editor::LineBuffer lb;
  for (size_t i = 0; i < len; ++i)
    lb.Append(data[i]);
  std::string line;
  std::cerr << lb.CountLines() << " " << lb.CountChars() << std::endl;
  for (size_t i = 0; i < lb.CountLines(); ++i) {
    line.clear();
    lb.GetLine(i, ~0, line);
    std::cout << line;
  }
  return 0;
}
