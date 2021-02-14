#include "support/defer.h"
#include "support/misc.h"
#include "support/sys.h"
#include "tui/renderer.h"
#include "tui/view.h"

int main(int argc, char *argv[]) {
  using namespace emcc;
  using namespace emcc::tui;
  if (argc != 4)
    Die("Usage: %s <filename> <start_line> <end_line>", argv[0]);
  std::string filename(argv[1]);
  size_t start_line = std::stoul(argv[2]);
  size_t end_line = std::stoul(argv[3]);
  auto buffer = MonoBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename);
  auto start = std::chrono::high_resolution_clock::now();
  auto end = std::chrono::high_resolution_clock::now();
  size_t nr_frames = 0;
  {
    initscr();
    defer { endwin(); };
    cbreak();
    noecho();
    NcursesRenderer renderer(stdscr);
    int height, width;
    renderer.GetMaxYX(height, width);
    Framebuffer framebuffer(width, height);
    Page page(buffer.get(), &framebuffer, width, height);
    start = std::chrono::high_resolution_clock::now();
    size_t total_lines = buffer->CountLines();
    for (size_t i = std::min(total_lines - 1, start_line);
         i < std::min(total_lines, end_line); ++i) {
      page.Reload(i);
      page.FillFrame(Cursor(0, 0), page.GetBoundary());
      renderer.Clear();
      renderer.RenderRange(framebuffer, Cursor(0, 0), page.GetBoundary());
      renderer.DrawCursor(Cursor(0, 0));
      renderer.Refresh();
      ++nr_frames;
    }
    end = std::chrono::high_resolution_clock::now();
  }
  float secs =
      1 / 1e6 *
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();
  std::cout << "fps: " << nr_frames / secs << std::endl;
  return 0;
}
