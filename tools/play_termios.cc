#include "support/sys.h"

#include <iostream>
#include <termios.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  using namespace emcc;
  struct termios t;
  tcgetattr(STDIN_FILENO, &t);
  std::cout << t.c_iflag << std::endl;
  std::cout << t.c_oflag << std::endl;
  std::cout << t.c_cflag << std::endl;
  std::cout << t.c_lflag << std::endl;
  return 0;
}
