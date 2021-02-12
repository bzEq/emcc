#pragma once

#include "tui/basis.h"
#include "tui/view.h"

#include <ncurses.h>

namespace emcc::tui {

class NcursesRenderer {
public:
  NcursesRenderer(WINDOW *window) : window_(window) {}

  void GetMaxYX(int &y, int &x) { getmaxyx(window_, y, x); }

  void Clear() { wclear(window_); }

  void RenderFull(const Framebuffer &fb);

  void RenderRange(const Framebuffer &fb, Cursor begin, Cursor end);

  void DrawCursor(Cursor c);

  void Refresh() { wrefresh(window_); }

private:
  WINDOW *window_;
};

} // namespace emcc::tui
