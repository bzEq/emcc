#include "tui/terminal.h"

namespace emcc::tui {

std::vector<std::function<void(void)>> ANSITerminal::atexit_functions;

void ANSITerminal::Clean() {
  while (!atexit_functions.empty()) {
    atexit_functions.back()();
    atexit_functions.pop_back();
  }
}

void ANSITerminal::RegisterAtExitCleaning() {
  std::atexit(ANSITerminal::Clean);
}

bool ANSITerminal::EnableRawMode() const {
  struct termios raw;
  tcgetattr(in(), &raw);
  atexit_functions.emplace_back(
      [=, fd = in()] { tcsetattr(fd, TCSANOW, &raw); });
  raw.c_iflag &=
      ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | INLCR | IGNCR | ICRNL);
  raw.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET);
  raw.c_lflag &=
      ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK | ECHONL | NOFLSH |
        TOSTOP | ECHOCTL | ECHOPRT | ECHOKE | FLUSHO | PENDIN | IEXTEN);
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;
  if (tcsetattr(in(), TCSAFLUSH, &raw) != 0)
    return false;
  return true;
}

} // namespace emcc::tui
