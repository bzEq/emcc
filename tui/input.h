#pragma once

#include <ncurses.h>

namespace emcc::tui {

class NcursesInput {
public:
  explicit NcursesInput(WINDOW *window) : window_(window) {}
  int GetNext() { return wgetch(window_); }

private:
  WINDOW *window_;
};

} // namespace emcc::tui
