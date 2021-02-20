#include "tui/renderer.h"
#include "tui/buffer_view.h"
#include "tui/cursor.h"

namespace emcc::tui {

void NcursesRenderer::RenderRegion(const BufferView &view, Region region) {
  RenderRegionAt({0, 0}, view, region);
}

void NcursesRenderer::RenderRegionAt(Cursor anchor, const BufferView &view,
                                     Region view_region) {
  Region render_region(region());
  if (!render_region.contains(anchor))
    return;
  for (auto c : view_region) {
    Cursor t = anchor + c;
    if (!render_region.contains(t))
      break;
    const Pixel &p = view.GetPixel(c);
    mvwaddch(window_, t.y, t.x, p.shade.character);
  }
}

void NcursesRenderer::RenderStringAt(Cursor at, const std::string &content) {
  wmove(window_, at.y, 0);
  wclrtoeol(window_);
  mvwprintw(window_, at.y, at.x, "%s", content.c_str());
}

void NcursesRenderer::DrawCursor(Cursor c) {
  if (region().contains(c))
    wmove(window_, c.y, c.x);
}

} // namespace emcc::tui
