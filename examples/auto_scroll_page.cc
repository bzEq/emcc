#include "editor/buffer_view.h"
#include "support/defer.h"
#include "support/misc.h"
#include "support/sys.h"
#include "tui/terminal.h"

#include <chrono>
#include <ncurses.h>
#include <thread>

static void Render(emcc::editor::BufferView &view, int height, int width) {
  wclear(stdscr);
  int y = 0;
  for (auto row : emcc::make_range(view.row_begin(), view.row_end())) {
    int x = 0;
    for (auto &cv : row) {
      mvwaddch(stdscr, y, x, cv.rune);
      x += cv.width;
      if (x >= width)
        break;
    }
    ++y;
    if (y >= height)
      break;
  }
  wrefresh(stdscr);
}

int main(int argc, char *argv[]) {
  using namespace emcc;
  using namespace emcc::editor;
  if (argc != 4)
    Die("Usage: {} <filename> <start_line> <end_line>", argv[0]);
  std::string filename(argv[1]);
  size_t start_line = std::stoul(argv[2]);
  size_t end_line = std::stoul(argv[3]);
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
    using namespace std::chrono_literals;
    initscr();
    defer { endwin(); };
    cbreak();
    noecho();
    size_t total_lines = buffer->CountLines();
    for (size_t i = std::min(total_lines - 1, start_line);
         i < std::min(total_lines, end_line); ++i) {
      view.RePosition(i);
      Render(view, height, width);
      std::this_thread::sleep_for(30ms);
    }
  }
  return 0;
}
