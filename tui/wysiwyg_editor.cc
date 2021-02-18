#include "tui/wysiwyg_editor.h"

#include <csignal>

namespace emcc::tui {

void WYSIWYGEditor::Show() {
  Cursor show_begin, show_end;
  std::tie(show_begin, show_end) = page_->GetRenderRange();
  renderer_->RenderRange(*page_, show_begin, show_end);
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
