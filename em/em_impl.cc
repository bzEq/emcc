#include "em.h"

namespace emcc {

using namespace emcc::tui;

Cursor Window::ToBufferViewCursor(Cursor c) {
  return {view_reference_row_ + c.y, c.x};
}

Cursor Window::ToWindowCursor(Cursor c) {
  assert(c.y >= view_reference_row_);
  return {c.y - view_reference_row_, c.x};
}

bool Window::LeftFindValidCharView(Cursor &c) {
  auto bvc = ToBufferViewCursor(c);
  auto it = view_.GetRow(bvc.y);
  if (!it)
    return false;
  int x = 0, px = 0;
  for (auto &cv : *it) {
    px = x;
    x += cv.length();
    if (x > bvc.x) {
      bvc.x = px;
      break;
    }
  }
  if (bvc.x == px) {
    c = ToWindowCursor(bvc);
    return true;
  }
  return false;
}

bool Window::MoveUp() {
  Cursor tmp = c_;
  if (tmp.y == 0) {
    ScrollDown(1);
  } else {
    --tmp.y;
  }
  if (LeftFindValidCharView(tmp)) {
    c_ = tmp;
    return true;
  }
  return false;
}

bool Window::MoveRight() {
  auto bvc = ToBufferViewCursor(c_);
  auto it = view_.GetRow(bvc.x);
  if (!it)
    return false;
  int x = 0;
  for (auto &cv : *it) {
    x += cv.length();
    if (x > bvc.x) {
      bvc.x = x;
      break;
    }
  }
  if (bvc.x == x) {
    c_ = ToWindowCursor(bvc);
    return true;
  }
  return false;
}

bool Window::MoveDown() {
  Cursor tmp = c_;
  if (tmp.y == buffer_height() - 1) {
    ScrollDown(1);
  } else {
    ++tmp.y;
  }
  if (LeftFindValidCharView(tmp)) {
    c_ = tmp;
    return true;
  }
  return false;
}

bool Window::MoveLeft() {
  auto bvc = ToBufferViewCursor(c_);
  auto it = view_.GetRow(bvc.x);
  if (!it)
    return false;
  int x = 0, px = 0;
  for (auto &cv : *it) {
    px = x;
    x += cv.length();
    if (x >= bvc.x) {
      bvc.x = px;
      break;
    }
  }
  if (bvc.x == px) {
    c_ = ToWindowCursor(bvc);
    return true;
  }
  return false;
}

void Window::ScrollUp(int num_rows) {
  view_reference_row_ = std::max(0, view_reference_row_ - num_rows);
}

void Window::ScrollDown(int num_rows) {
  view_.Resize(view_reference_row_ + buffer_height() + num_rows, width_);
  view_reference_row_ =
      std::min((int)view_.NumRows(), view_reference_row_ + num_rows);
}

} // namespace emcc
