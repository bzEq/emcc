#include "tui/wysiwyg_editor.h"
#include "support/epoll.h"

#include <csignal>

namespace emcc::tui {

void WYSIWYGEditor::Show() {
  Region view_region = page_->GetDiffRegion(0, 0);
  renderer_->RenderRegion(*page_, view_region);
  renderer_->RenderStringAt({(int)page_->height(), 0}, page_->GetStatusLine());
  renderer_->DrawCursor(page_->cursor());
  renderer_->Refresh();
}

int WYSIWYGEditor::Run() {
  Resize();
  EPoll ep;
  ep.MonitorReadEvent(signal_queue_->receive_chan());
  ep.MonitorReadEvent(input_->fd());
  std::vector<epoll_event> events(16);
  while (!have_to_stop_) {
    Show();
    int num_events;
    if (!ep.Wait(-1, &events, &num_events)) {
      if (errno == EINTR)
        continue;
      status_ = -1;
      break;
    }
    assert(num_events <= events.size());
    for (int i = 0; i < num_events; ++i) {
      auto &event = events[i];
      if (event.data.fd == signal_queue_->receive_chan()) {
        // Handle external signals.
        int signum;
        if (signal_queue_->get(signum)) {
          if (signum == SIGWINCH) {
            Resize();
          }
        }
      } else if (event.data.fd == input_->fd()) {
        // Handle input.
        int ch = input_->GetNext();
        Consume(ch);
      }
    }
  }
  return status_;
}

void WYSIWYGEditor::Resize() {
  int height, width;
  struct winsize ws;
  ioctl(renderer_->fd(), TIOCGWINSZ, &ws);
  height = ws.ws_row;
  width = ws.ws_col;
  renderer_->Resize(height, width);
  renderer_->Clear();
  page_->Resize(height - 1, width);
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
