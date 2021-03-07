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

// See http://ascii-table.com/ansi-escape-sequences.php
class ANSITerminal {
public:
  static std::vector<std::function<void(void)>> atexit_functions;
  static void Clean();
  static void RegisterAtExitCleaning();

  ANSITerminal(int in, int out) : in_(in), out_(out) {}

  bool EnableRawMode() const;

  int in() const { return in_; }
  int out() const { return out_; }

  void GetMaxYX(int &y, int &x) {
    struct winsize ws;
    ioctl(out(), TIOCGWINSZ, &ws);
    y = ws.ws_row;
    x = ws.ws_col;
  }

  void MoveCursor(Cursor c) {
    std::string cs(fmt::format("\x1B[{};{}H", c.y, c.x));
    ::write(out_, cs.data(), cs.length());
  }

  void Clear() {
    static const char cs[] = "\x1B[2J";
    ::write(out_, cs, GetArrayLength(cs));
  }

  void ClearLine() {
    static const char cs[] = "\x1B[K";
    ::write(out_, cs, GetArrayLength(cs));
  }

private:
  int in_, out_;
};

} // namespace emcc::tui
