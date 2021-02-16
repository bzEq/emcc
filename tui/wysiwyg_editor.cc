#include "tui/wysiwyg_editor.h"

#include <csignal>

namespace emcc::tui {

void WYSIWYGEditor::Show() {
  if (changed_) {
    page_->Reload();
    renderer_->RenderRange(page_->framebuffer(), Cursor(0, 0),
                           page_->GetBoundary());
    changed_ = false;
  }
  page_->UpdateStatusLine(loc_);
  renderer_->RenderRange(page_->framebuffer(), Cursor(page_->height() - 1, 0),
                         page_->GetBoundary());
  renderer_->DrawCursor(loc_);
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

void WYSIWYGEditor::Resize() {}

void WYSIWYGEditor::MoveUp() {
  Cursor probe = loc_;
  probe.y = std::max(loc_.y - 1, 0);
  Pixel pixel;
  if (page_->GetPixel(probe, pixel) && pixel.position.point != ~0)
    loc_ = probe;
}

void WYSIWYGEditor::MoveDown() {
  Cursor probe = loc_;
  probe.y = std::min(loc_.y + 1, (int)page_->height());
  Pixel pixel;
  if (page_->GetPixel(probe, pixel) && pixel.position.point != ~0)
    loc_ = probe;
}

void WYSIWYGEditor::MoveLeft() {
  Cursor probe = loc_;
  probe.x = std::max(loc_.x - 1, 0);
  Pixel pixel;
  if (page_->GetPixel(probe, pixel) && pixel.position.point != ~0)
    loc_ = probe;
}

void WYSIWYGEditor::MoveRight() {
  Cursor probe = loc_;
  probe.x = std::min((int)page_->width(), loc_.x + 1);
  Pixel pixel;
  if (page_->GetPixel(probe, pixel) && pixel.position.point != ~0)
    loc_ = probe;
}

void WYSIWYGEditor::Consume(int ch) {
  // Esc is pressed.
  if (ch == 27) {
    have_to_stop_ = true;
    return;
  }
  switch (ch) {
  case KEY_UP:
    MoveUp();
    break;
  case KEY_DOWN:
    MoveDown();
    break;
  case KEY_LEFT:
    MoveLeft();
    break;
  case KEY_RIGHT:
    MoveRight();
    break;
  default:
    break;
  }
  return;
}

} // namespace emcc::tui
