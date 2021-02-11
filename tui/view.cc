#include "tui/view.h"

#include <ctype.h>

namespace emcc::tui {

WYSIWYGEditor &WYSIWYGEditor::ReloadView(size_t start_line) {
  y_ = x_ = 0;
  for (size_t i = start_line; i < buffer_->CountLines(); ++i) {
    std::string content;
    buffer_->GetLine(i, ~0, content);
    for (size_t j = 0; j < content.size(); ++j) {
      int y = y_, x = x_;
      if (Write(content[j])) {
        view_->Set(y, x, i, j);
      }
    }
    if (y_ >= view_->height())
      break;
  }
  return *this;
}

bool WYSIWYGEditor::Write(int c) {
  if (y_ >= view_->height())
    return false;
  int c_print = c;
  if (!isprint(c_print)) {
    c_print = ' ';
  }
  if (c == '\t') {
    c_print = ' ';
  }
  mvwaddch(curse_win_, y_, x_, c_print);
  if (c == '\n') {
    x_ = 0;
    y_ += 1;
  } else {
    ++x_;
  }
  if (x_ >= view_->width()) {
    x_ = 0;
    y_ += 1;
  }
  wmove(curse_win_, y_, x_);
  wrefresh(curse_win_);
  return true;
}

} // namespace emcc::tui
