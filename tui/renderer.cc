#include "tui/renderer.h"
#include "tui/cursor.h"

namespace emcc::tui {

void NcursesRenderer::RenderRange(const Framebuffer &fb, Cursor begin,
                                  Cursor end) {
  int max_y, max_x;
  GetMaxYX(max_y, max_x);
  Cursor boundary(max_y, max_x);
  for (Cursor c = begin; c != end; c = Cursor::Goto(fb.width(), c, 1)) {
    if (Cursor::IsBeyond(c, boundary) || Cursor::IsBeyond(c, fb.GetBoundary()))
      break;
    const Pixel &p = fb.Get(c.y, c.x);
    mvwaddch(window_, c.y, c.x, p.shade.character);
  }
}

void NcursesRenderer::RenderFull(const Framebuffer &fb) {
  Clear();
  Cursor begin(0, 0), end(fb.height(), fb.width());
  RenderRange(fb, begin, end);
}

void NcursesRenderer::DrawCursor(Cursor c) {
  int max_y, max_x;
  GetMaxYX(max_y, max_x);
  Cursor boundary(max_y, max_x);
  if (!Cursor::IsBeyond(c, boundary))
    wmove(window_, c.y, c.x);
}

} // namespace emcc::tui
