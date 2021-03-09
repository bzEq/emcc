#include "editor/buffer_view.h"
#include "support/defer.h"
#include "support/misc.h"
#include "support/sys.h"
#include "tui/terminal.h"

#include <chrono>
#include <ncurses.h>
#include <thread>

static void Render(emcc::editor::BufferView &view, int height, int width) {
  int y = 0;
  for (auto row : emcc::make_range(view.row_begin(), view.row_end())) {
    int x = 0;
    for (auto &cv : row) {
      mvaddch(y, x, cv.rune);
      x += cv.width;
      if (x >= width)
        break;
    }
    ++y;
    if (y >= height)
      break;
  }
  refresh();
}

int main(int argc, char *argv[]) {
  using namespace emcc;
  using namespace emcc::editor;
  if (argc != 3)
    Die("Usage: {} <filename> <start_line>", argv[0]);
  std::string filename(argv[1]);
  size_t start_line = std::stoul(argv[2]);
  auto buffer = editor::MonoBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename.c_str());
  if (!buffer->IsUTF8Encoded())
    Die("Unable to handle non-utf8 encoded files");
  emcc::tui::ANSITerminal vt(STDIN_FILENO, STDOUT_FILENO);
  int height, width;
  vt.GetMaxYX(height, width);
  BufferView view(buffer.get(), height, width);
  {
    initscr();
    defer { endwin(); };
    cbreak();
    noecho();
    view.RePosition(start_line);
    Render(view, height, width);
    while (true) {
      int ch = getch();
      if (ch == '\x1b')
        break;
    }
  }
  return 0;
}
