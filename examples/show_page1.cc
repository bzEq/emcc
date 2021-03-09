#include "editor/buffer_view.h"
#include "support/defer.h"
#include "support/misc.h"
#include "support/sys.h"
#include "tui/terminal.h"

#include <chrono>
#include <codecvt>
#include <locale>
#include <string>
#include <thread>

static void Render(emcc::tui::ANSITerminal &vt, emcc::editor::BufferView &view,
                   int height, int width) {
  auto to_string = [](const std::wstring &s) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(s);
  };
  vt.Clear();
  int y = 0;
  for (auto row : emcc::make_range(view.row_begin(), view.row_end())) {
    int x = 0;
    for (auto &cv : row) {
      std::wstring ws;
      ws.push_back(cv.rune);
      std::string s(to_string(ws));
      vt.MoveCursor({y, x});
      vt.Put(s);
      x += cv.length();
    }
    ++y;
    if (y >= height)
      break;
  }
  vt.Refresh();
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
  emcc::tui::ANSITerminal::RegisterAtExitCleaning();
  emcc::tui::ANSITerminal vt(STDIN_FILENO, STDOUT_FILENO);
  int height, width;
  vt.GetMaxYX(height, width);
  BufferView view(buffer.get(), height, width);
  {
    using namespace std::chrono_literals;
    emcc::tui::ANSITerminal::EnableRawMode(vt.in());
    view.RePosition(start_line);
    Render(vt, view, height, width);
    std::this_thread::sleep_for(2000ms);
  }
  return 0;
}
