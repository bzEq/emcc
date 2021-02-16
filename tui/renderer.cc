#include "tui/renderer.h"
#include "tui/buffer_view.h"
#include "tui/cursor.h"

namespace emcc::tui {

void NcursesRenderer::RenderRange(const BufferView &view, Cursor begin,
                                  Cursor end) {
  RenderRangeAt({0, 0}, view, begin, end);
}

void NcursesRenderer::RenderRangeAt(Cursor anchor, const BufferView &view,
                                    Cursor begin, Cursor end) {
  Cursor boundary(GetBoundary());
  if (Cursor::IsBeyond(anchor, boundary))
    return;
  Cursor view_boundary(view.GetBoundary());
  for (Cursor c = begin; !Cursor::IsBeyond(c, end);
       c = Cursor::Goto(view.width(), c, 1)) {
    if (Cursor::IsBeyond(c, view_boundary))
      continue;
    Cursor t = anchor + c;
    if (Cursor::IsBeyond(t, boundary))
      continue;
    const Pixel &p = view.GetPixel(c.y, c.x);
    mvwaddch(window_, t.y, t.x, p.shade.character);
  }
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
