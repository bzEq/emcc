#include "tui/wysiwyg_editor.h"

namespace emcc::tui {

void WYSIWYGEditor::Show() {
  if (changed_) {
    page_->Reload(base_line_);
    page_->FillFrame(Cursor(0, 0), page_->GetBoundary());
    renderer_->RenderRange(page_->GetFrameBuffer(), Cursor(0, 0),
                           page_->GetBoundary());
    changed_ = false;
  }
  renderer_->DrawCursor(loc_);
  renderer_->Refresh();
}

} // namespace emcc::tui
