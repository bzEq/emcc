#include "tui/view.h"

#include <ctype.h>

namespace emcc::tui {

WYSIWYGEditor &WYSIWYGEditor::ReloadView(size_t start_line) {
  y_ = x_ = 0;
  for (size_t i = start_line; i < buffer_->CountLines(); ++i) {
    std::string content;
    buffer_->GetLine(i, ~0, content);
    for (auto c : content) {
      Write(c);
    }
    if (y_ >= view_->height())
      break;
  }
  return *this;
}

WYSIWYGEditor &WYSIWYGEditor::Write(int c) { return *this; }

} // namespace emcc::tui
