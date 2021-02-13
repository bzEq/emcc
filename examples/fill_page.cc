#include "support/misc.h"
#include "support/sys.h"
#include "tui/renderer.h"
#include "tui/view.h"

#include <chrono>

int main(int argc, char *argv[]) {
  using namespace emcc;
  using namespace emcc::tui;
  if (argc != 2)
    Die("Usage: %s <filename>", argv[0]);
  std::string filename(argv[1]);
  auto buffer = MonoBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename);
  int height = 40, width = 120;
  Framebuffer framebuffer(width, height);
  Page page(buffer.get(), &framebuffer, width, height);
  auto start = std::chrono::high_resolution_clock::now();
  page.Reload(0);
  page.FillFrame(Cursor(0, 0), page.GetBoundary());
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Elapsed time in microseconds : "
            << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                     start)
                   .count()
            << " µs" << std::endl;
  return 0;
}
