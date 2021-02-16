#include "tui/renderer.h"
#include "tui/cursor.h"

namespace emcc::tui {

void NcursesRenderer::RenderRange(const Framebuffer &fb, Cursor begin,
                                  Cursor end) {
  RenderRangeAt({0, 0}, fb, begin, end);
}

void NcursesRenderer::RenderRangeAt(Cursor anchor, const Framebuffer &fb,
                                    Cursor begin, Cursor end) {
  Cursor boundary(GetBoundary());
  if (Cursor::IsBeyond(anchor, boundary))
    return;
  Cursor fb_boundary(fb.GetBoundary());
  for (Cursor c = begin; !Cursor::IsBeyond(c, end);
       c = Cursor::Goto(fb.width(), c, 1)) {
    if (Cursor::IsBeyond(c, fb_boundary))
      continue;
    Cursor t = anchor + c;
    if (Cursor::IsBeyond(t, boundary))
      continue;
    const Pixel &p = fb.Get(c.y, c.x);
    mvwaddch(window_, t.y, t.x, p.shade.character);
  }
}

void NcursesRenderer::RenderFull(const Framebuffer &fb) {
  Clear();
  RenderRange(fb, {0, 0}, fb.GetBoundary());
}

Cursor NcursesRenderer::GetBoundary() {
  int max_y, max_x;
  GetMaxYX(max_y, max_x);
  return Cursor(max_y, max_x);
}

void NcursesRenderer::DrawCursor(Cursor c) {
  if (!Cursor::IsBeyond(c, GetBoundary()))
    wmove(window_, c.y, c.x);
}

} // namespace emcc::tui
