#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "support/misc.h"
#include "support/sys.h"
#include "tui/terminal.h"

#include <functional>
#include <stdio.h>
#include <vector>

static std::vector<std::function<void(void)>> atexit_functions;

static void Clean() {
  while (!atexit_functions.empty()) {
    atexit_functions.back()();
    atexit_functions.pop_back();
  }
}

int main() {
  using namespace emcc;
  using namespace emcc::tui;
  std::atexit(Clean);
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw);
  atexit_functions.emplace_back(
      [=] { tcsetattr(STDIN_FILENO, TCSANOW, &raw); });
  raw.c_iflag &=
      ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | INLCR | IGNCR | ICRNL);
  raw.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET);
  raw.c_lflag &=
      ~(ISIG | ICANON | XCASE | ECHO | ECHOE | ECHOK | ECHONL | NOFLSH |
        TOSTOP | ECHOCTL | ECHOPRT | ECHOKE | FLUSHO | PENDIN | IEXTEN);
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0;
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0)
    Die("Failed to enable raw mode");
  ANSITerminal vt(STDIN_FILENO, STDOUT_FILENO);
  vt.Clear();
  vt.MoveCursor({16, 0});
  static const char msg[] = "你好世界\n";
  ::write(vt.out(), msg, GetArrayLength(msg));
  sleep(5);
  return 0;
}
