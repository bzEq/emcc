#pragma once

#include <sys/ioctl.h>

#include "tui/buffer_view.h"
#include "tui/cursor.h"

#include <ncurses.h>

namespace emcc::tui {

class NcursesRenderer {
public:
  NcursesRenderer(WINDOW *window) : window_(window) {}

  int fd() const { return STDOUT_FILENO; }

  void GetMaxYX(int &y, int &x) {
    getmaxyx(window_, y, x);
  }

  void Clear() { wclear(window_); }

  void RenderRegion(const BufferView &fb, Region region);

  void RenderRegionAt(Cursor anchor, const BufferView &view, Region region);

  void RenderStringAt(Cursor at, const std::string &content);

  void DrawCursor(Cursor c);

  void Resize(int height, int width) {
    wresize(window_, height, width);
  }

  void Refresh() { wrefresh(window_); }

  Region region() {
    int y, x;
    GetMaxYX(y, x);
    return Region(x, {0, 0}, {y, 0});
  }

private:
  WINDOW *window_;
};

} // namespace emcc::tui
