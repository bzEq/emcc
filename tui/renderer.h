#pragma once

#include "tui/basis.h"

#include <ncurses.h>

namespace emcc::tui {

class NcursesRenderer {
public:
  NcursesRenderer(WINDOW *window) : window_(window) {}

  void GetMaxYX();

  void Clear() { wclear(window_); }

  void RenderFull(const Framebuffer &fb);

  void RenderRange(const Framebuffer &fb, Cursor begin, Cursor end);

  void RenderLine(const Framebuffer &fb, size_t l);

private:
  WINDOW *window_;
};

} // namespace emcc::tui
