// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "edit/mono_buffer.h"
#include "support/sys.h"
#include "tui/wysiwyg_editor.h"

using namespace emcc;
using namespace emcc::tui;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    Die("Usage: {} <filename>");
  }
  std::string filename(argv[1]);
  auto buffer = MonoBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename);
  initscr();
  atexit([]() { endwin(); });
  raw();
  noecho();
  NcursesRenderer renderer(stdscr);
  NcursesInput input(stdscr);
  int height, width;
  renderer.GetMaxYX(height, width);
  Framebuffer framebuffer(width, height);
  Page page(buffer.get(), &framebuffer, width, height);
  page.set_baseline(0);
  WYSIWYGEditor editor(&page, buffer.get(), &input, &renderer);
  return editor.Run();
}
