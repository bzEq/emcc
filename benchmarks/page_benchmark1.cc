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
  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  size_t nr_frames = 0;
  emcc::tui::ANSITerminal vt(STDIN_FILENO, STDOUT_FILENO);
  int height, width;
  vt.GetMaxYX(height, width);
  BufferView view(buffer.get(), height, width);
  {
    initscr();
    defer { endwin(); };
    cbreak();
    noecho();
    start = std::chrono::high_resolution_clock::now();
    size_t total_lines = buffer->NumLines();
    for (size_t i = std::min(total_lines - 1, start_line);
         i < std::min(total_lines, end_line); ++i) {
      view.RePosition(i);
      Render(view, height, width);
      ++nr_frames;
    }
    end = std::chrono::high_resolution_clock::now();
  }
  float secs =
      1 / 1e6 *
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();
  std::cout << "time elapsed: " << secs << std::endl;
  std::cout << "frames: " << nr_frames << std::endl;
  std::cout << "fps: " << nr_frames / secs << std::endl;
  return 0;
}
