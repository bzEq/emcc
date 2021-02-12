#include "support/misc.h"
#include "support/sys.h"
#include "tui/renderer.h"
#include "tui/view.h"

int main(int argc, char *argv[]) {
  using namespace emcc;
  using namespace emcc::tui;
  if (argc != 2)
    Die("Usage: %s <filename>", argv[0]);
  std::string filename(argv[1]);
  auto buffer = LineBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename);
  initscr();
  atexit([]() { endwin(); });
  raw();
  noecho();
  NcursesRenderer renderer(stdscr);
  size_t height = 40, width = 120;
  Framebuffer framebuffer(width, height);
  Page page(buffer.get(), &framebuffer, width, height);
  page.Reload(0);
  page.FillFrame(Cursor(0, 0), page.GetBoundary());
  renderer.RenderRange(framebuffer, Cursor(0, 0), page.GetBoundary());
  renderer.DrawCursor(Cursor(0, 0));
  renderer.Refresh();
  while (true) {
    if (getch() == 27)
      break;
  }
  return 0;
}
