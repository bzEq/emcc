#include "support/misc.h"
#include "support/sys.h"
#include "tui/view.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    emcc::Die("Usage: %s <filename>", argv[0]);
  }
  std::string filename(argv[1]);
  auto membuf = emcc::MemoryBuffer::OpenIfExists(filename);
  auto data = membuf->buffer();
  size_t len = membuf->length();
  emcc::LineBuffer lb;
  for (size_t i = 0; i < len; ++i)
    lb.Append(data[i]);
  initscr();
  atexit([]() { endwin(); });
  raw();
  noecho();
  keypad(stdscr, false);
  emcc::tui::Viewport view(80, 24);
  emcc::tui::WYSIWYGEditor editor(stdscr, &view, &lb);
  editor.ReloadView(0);
  while (true) {
    int c = getch();
    if (c == 27)
      break;
  }
  return 0;
}
