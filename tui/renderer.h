#pragma once

#include "tui/buffer_view.h"
#include "tui/cursor.h"

#include <ncurses.h>

namespace emcc::tui {

class NcursesRenderer {
public:
  NcursesRenderer(WINDOW *window) : window_(window) {}

  void GetMaxYX(int &y, int &x) { getmaxyx(window_, y, x); }

  void Clear() { wclear(window_); }

  void RenderRange(const BufferView &fb, Cursor begin, Cursor end);

  void RenderRangeAt(Cursor anchor, const BufferView &view, Cursor begin,
                     Cursor end);

  void DrawCursor(Cursor c);

  void Refresh() { wrefresh(window_); }

  Cursor GetBoundary();

private:
  WINDOW *window_;
};

} // namespace emcc::tui
