#include "tui/renderer.h"

#include <codecvt>
#include <locale>
#include <string>
#include <wchar.h>

namespace emcc::tui {

void RenderBufferViewWithNCurses(emcc::editor::BufferView &view, WINDOW *window,
                                 int height, int width) {
  int y = 0;
  for (auto row : emcc::make_range(view.row_begin(), view.row_end())) {
    int x = 0;
    for (auto &cv : row) {
      mvwaddch(window, y, x, cv.rune);
      x += cv.width;
      if (x >= width)
        break;
    }
    ++y;
    if (y >= height)
      break;
  }
  wrefresh(window);
}

static std::string wstring_to_string(const std::wstring &s) {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  return converter.to_bytes(s);
}

void RenderBufferView(emcc::editor::BufferView &view,
                      emcc::tui::ANSITerminal &vt, int height, int width) {
  vt.Clear();
  int y = 0;
  for (auto row : emcc::make_range(view.row_begin(), view.row_end())) {
    int x = 0;
    for (auto &cv : row) {
      std::wstring ws;
      ws.push_back(cv.rune);
      std::string s(wstring_to_string(ws));
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

} // namespace emcc::tui
