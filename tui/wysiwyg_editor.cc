#include "tui/wysiwyg_editor.h"

#include <csignal>

namespace emcc::tui {

void WYSIWYGEditor::Show() {
  Region view_region = page_->GetDiffRegion(0, 0);
  renderer_->RenderRegion(*page_, view_region);
  renderer_->DrawCursor(page_->cursor());
  renderer_->Refresh();
}

int WYSIWYGEditor::Run() {
  Resize();
  while (!have_to_stop_) {
    Show();
    // Handle external signal.
    int signum;
    if (signal_queue_->get_nowait(signum)) {
      if (signum == SIGWINCH) {
        Resize();
        continue;
      }
    }
    int ch = input_->GetNext();
    Consume(ch);
  }
  return status_;
}

void WYSIWYGEditor::Resize() {
  int height, width;
  renderer_->GetMaxYX(height, width);
  if (width != page_->width() || height != page_->height()) {
    // Size changed.
    page_->Resize(height, width);
  }
}

void WYSIWYGEditor::Consume(int ch) {
  // Esc is pressed.
  if (ch == 27) {
    have_to_stop_ = true;
    return;
  }
  switch (ch) {
  case 'k':
    page_->MoveUp();
    break;
  case 'j':
    page_->MoveDown();
    break;
  case 'h':
    page_->MoveLeft();
    break;
  case 'l':
    page_->MoveRight();
    break;
  default:
    break;
  }
  return;
}

} // namespace emcc::tui
