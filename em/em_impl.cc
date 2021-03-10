#include <unistd.h>

#include "em.h"
#include "support/epoll.h"
#include "support/sys.h"

#include <codecvt>
#include <locale>

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
  view_.Extend(view_reference_row_ + buffer_height() + num_rows);
  view_reference_row_ =
      std::min((int)view_.NumRows(), view_reference_row_ + num_rows);
}

int Window::Run() {
  EPoll m;
  std::vector<epoll_event> events(16);
  emcc::SetNonBlocking(STDIN_FILENO);
  m.MonitorReadEvent(STDIN_FILENO);
  char buf[4096];
  while (!have_to_stop_) {
    Show();
    int num_events;
    if (m.Wait(-1, &events, &num_events)) {
      if (errno == EINTR)
        continue;
      status_ = -1;
      break;
    }
    assert(num_events <= events.size());
    for (int i = 0; i < num_events; ++i) {
      if (have_to_stop_)
        break;
      auto &event = events[i];
      if (event.data.fd == STDIN_FILENO) {
        while (true) {
          int nread = ::read(event.data.fd, buf, GetArrayLength(buf));
          if (nread < 0 && errno != EAGAIN) {
            have_to_stop_ = true;
            status_ = -1;
            break;
          }
          Consume(buf, nread);
        }
      }
    }
  }
  return status_;
}

void Window::Consume(const char *input, int n) {
  for (int i = 0; i < n; ++i) {
    if (input[i] == '\x1B') {
      have_to_stop_ = true;
      break;
    }
    switch (input[i]) {
    case 'k':
      MoveUp();
      break;
    case 'j':
      MoveDown();
      break;
    case 'h':
      MoveLeft();
      break;
    case 'l':
      MoveRight();
      break;
    }
  }
}

void Window::Show() {
  auto to_string = [](const std::wstring &s) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(s);
  };
  vt_.Clear();
  int y = 0;
  // for (auto row : emcc::make_range(view_.row_begin(), view_.row_end())) {
  //   int x = 0;
  //   for (auto &cv : row) {
  //     std::wstring ws;
  //     ws.push_back(cv.rune);
  //     std::string s(to_string(ws));
  //     vt_.MoveCursor({y, x});
  //     vt_.Put(s);
  //     x += cv.length();
  //   }
  //   ++y;
  //   if (y >= height_)
  //     break;
  // }
  vt_.Refresh();
}

} // namespace emcc
