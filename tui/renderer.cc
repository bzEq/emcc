#include "tui/renderer.h"
#include "tui/basis.h"
#include "tui/view.h"

namespace emcc::tui {

void NcursesRenderer::RenderRange(const Framebuffer &fb, Cursor begin,
                                  Cursor end) {
  int max_y, max_x;
  GetMaxYX(max_y, max_x);
  Cursor boundary(max_y, max_x);
  for (Cursor c = begin; c != end; c = JumpTo(fb.width(), c, 1)) {
    if (c >= boundary)
      break;
    Pixel p = fb.Get(c.y, c.x);
    mvwaddch(window_, c.y, c.x, p.character);
  }
}

void NcursesRenderer::RenderFull(const Framebuffer &fb) {
  Clear();
  Cursor begin(0, 0), end(fb.height(), fb.width());
  RenderRange(fb, begin, end);
}

} // namespace emcc::tui
