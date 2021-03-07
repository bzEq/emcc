#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "support/misc.h"
#include "support/sys.h"
#include "tui/terminal.h"

#include <functional>
#include <stdio.h>
#include <vector>

int main() {
  using namespace emcc;
  using namespace emcc::tui;
  ANSITerminal::RegisterAtExitCleaning();
  ANSITerminal vt(STDIN_FILENO, STDOUT_FILENO);
  if (!vt.EnableRawMode())
    Die("Failed to enable raw mode for {}", vt.in());
  vt.Clear();
  vt.MoveCursor({16, 0});
  static const char msg[] = "你好世界\n";
  ::write(vt.out(), msg, GetArrayLength(msg));
  sleep(3);
  return 0;
}
