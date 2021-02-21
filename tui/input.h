#pragma once

#include <ncurses.h>
#include <unistd.h>

namespace emcc::tui {

class NcursesInput {
public:
  explicit NcursesInput(WINDOW *window) : window_(window) {}
  int GetNext() { return wgetch(window_); }
  int fd() const { return STDIN_FILENO; }

private:
  WINDOW *window_;
};

} // namespace emcc::tui
