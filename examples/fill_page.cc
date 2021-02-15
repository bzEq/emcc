#include "support/misc.h"
#include "support/sys.h"
#include "tui/page.h"
#include "tui/renderer.h"

#include <chrono>

int main(int argc, char *argv[]) {
  using namespace emcc;
  using namespace emcc::tui;
  if (argc != 3)
    Die("Usage: {} <filename> <start_line>", argv[0]);
  std::string filename(argv[1]);
  size_t start_line = std::stoul(argv[2]);
  auto buffer = MonoBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename);
  int height = 40, width = 120;
  Framebuffer framebuffer(width, height);
  Page page(buffer.get(), &framebuffer, width, height);
  auto start = std::chrono::high_resolution_clock::now();
  page.set_baseline(start_line);
  page.Reload();
  auto end = std::chrono::high_resolution_clock::now();
  std::cout << "Elapsed time in microseconds : "
            << std::chrono::duration_cast<std::chrono::microseconds>(end -
                                                                     start)
                   .count()
            << " µs" << std::endl;
  return 0;
}
