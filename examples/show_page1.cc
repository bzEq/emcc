#include "support/defer.h"
#include "support/misc.h"
#include "support/sys.h"
#include "tui/renderer.h"

#include <chrono>
#include <codecvt>
#include <locale>
#include <string>
#include <thread>

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
  emcc::tui::ANSITerminal::RegisterAtExitCleaning();
  emcc::tui::ANSITerminal vt(STDIN_FILENO, STDOUT_FILENO);
  int height, width;
  vt.GetMaxYX(height, width);
  BufferView view(buffer.get(), height, width);
  {
    using namespace std::chrono_literals;
    emcc::tui::ANSITerminal::EnableRawMode(vt.in());
    view.RePosition(start_line);
    emcc::tui::RenderBufferView(view, vt, height, width);
    std::this_thread::sleep_for(2000ms);
  }
  return 0;
}
