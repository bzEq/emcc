#include "tui/wysiwyg_editor.h"

#include <csignal>

namespace emcc::tui {

void WYSIWYGEditor::Show() {
  if (changed_) {
    int height, width;
    renderer_->GetMaxYX(height, width);
    page_->set_width(width);
    page_->FillFramebuffer(height);
    renderer_->RenderRange(*page_, Cursor(0, 0), page_->GetBoundary());
    changed_ = false;
  }
  renderer_->DrawCursor(page_->cursor());
  renderer_->Refresh();
}

int WYSIWYGEditor::Run() {
  changed_ = true;
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

void WYSIWYGEditor::Resize() { changed_ = true; }

void WYSIWYGEditor::Consume(int ch) {
  // Esc is pressed.
  if (ch == 27) {
    have_to_stop_ = true;
    return;
  }
  switch (ch) {
  case KEY_UP:
    page_->MoveUp();
    break;
  case KEY_DOWN:
    page_->MoveDown();
    break;
  case KEY_LEFT:
    page_->MoveLeft();
    break;
  case KEY_RIGHT:
    page_->MoveRight();
    break;
  default:
    break;
  }
  return;
}

} // namespace emcc::tui
