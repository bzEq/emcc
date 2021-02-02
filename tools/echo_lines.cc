// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "editor/line_buffer.h"
#include "support/sys.h"

#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  if (argc < 2)
    emcc::Die("Expect a file name.");
  std::string filename(argv[1]);
  auto membuf = emcc::MemoryBuffer::OpenIfExists(filename);
  auto data = membuf->buffer();
  size_t len = membuf->length();
  emcc::LineBuffer lb;
  for (size_t i = 0; i < len; ++i)
    lb.Append(data[i]);
  std::string line;
  for (size_t i = 0; i < lb.CountLines(); ++i) {
    line.clear();
    lb.GetLine(i, ~0, &line);    
    std::cout << line;
  }
  return 0;
}
