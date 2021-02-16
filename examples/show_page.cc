#include "support/defer.h"
#include "support/misc.h"
#include "support/sys.h"
#include "tui/renderer.h"

#include <chrono>
#include <thread>

int main(int argc, char *argv[]) {
  using namespace emcc;
  using namespace emcc::tui;
  if (argc != 3)
    Die("Usage: {} <filename> <start_line>", argv[0]);
  std::string filename(argv[1]);
  size_t start_line = std::stoul(argv[2]);
  auto buffer = MonoBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename.c_str());
  {
    initscr();
    defer { endwin(); };
    cbreak();
    noecho();
    NcursesRenderer renderer(stdscr);
    int height, width;
    renderer.GetMaxYX(height, width);
    BufferView view(buffer.get());
    view.set_baseline(start_line);
    view.FillFramebuffer(height);
    renderer.RenderRange(view, {0, 0}, view.GetBoundary());
    renderer.DrawCursor({0, 0});
    while (true) {
      int ch = getch();
      if (ch == 27)
        break;
    }
  }
  return 0;
}
