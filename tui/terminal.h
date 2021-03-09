#pragma once

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "fmt/format.h"
#include "support/misc.h"
#include "support/wcwidth.h"
#include "tui/cursor.h"

#include <functional>
#include <vector>

namespace emcc::tui {

// FIXME: Need to investigate why is ncurses is much faster by comparing
// benchmarks/page_benchmark2 and benchmarks/page_benchmark3.
// See http://ascii-table.com/ansi-escape-sequences.php
class ANSITerminal {
public:
  static std::vector<std::function<void(void)>> atexit_functions;
  static void Clean();
  static void RegisterAtExitCleaning();
  static bool EnableRawMode(int fd, bool recover_atexit = true);

  ANSITerminal(int in, int out) : in_(in), out_(out) {}

  int in() const { return in_; }
  int out() const { return out_; }

  void GetMaxYX(int &y, int &x) {
    struct winsize ws;
    ioctl(out(), TIOCGWINSZ, &ws);
    y = ws.ws_row;
    x = ws.ws_col;
  }

  ANSITerminal &MoveCursor(Cursor c) {
    command_.append(fmt::format("\x1B[{};{}H", c.y, c.x));
    return *this;
  }

  ANSITerminal &Clear() {
    static const char cs[] = "\x1B[2J";
    command_.append(cs);
    return *this;
  }

  ANSITerminal &ClearLine() {
    static const char cs[] = "\x1B[K";
    command_.append(cs);
    return *this;
  }

  void Refresh() {
    ::write(out_, command_.data(), command_.length());
    command_.clear();
  }

  ANSITerminal &Put(const std::string &cs) {
    command_.append(cs);
    return *this;
  }

private:
  int in_, out_;
  std::string command_;
};

} // namespace emcc::tui
