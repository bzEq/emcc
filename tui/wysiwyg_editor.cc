#include "tui/wysiwyg_editor.h"

namespace emcc::tui {

void WYSIWYGEditor::Show() {
  if (changed_) {
    page_->Reload();
    renderer_->RenderRange(page_->framebuffer(), Cursor(0, 0),
                           page_->GetBoundary());
    changed_ = false;
  }
  renderer_->DrawCursor(loc_);
  renderer_->Refresh();
}

int WYSIWYGEditor::Run() {
  changed_ = true;
  while (!have_to_stop_) {
    Show();
    int ch = input_->GetNext();
    Handle(ch);
  }
  return status_;
}

void WYSIWYGEditor::Handle(int ch) {
  // Esc is pressed.
  if (ch == 27) {
    have_to_stop_ = true;
  }
}

} // namespace emcc::tui
