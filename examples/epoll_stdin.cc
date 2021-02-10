// Copyright (c) 2020 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include <fcntl.h>
#include <unistd.h>

#include "fmt/format.h"
#include "support/epoll.h"
#include "support/misc.h"
#include "support/sys.h"

#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

using namespace emcc;

static struct termios old_mode;

static void RestoreOldMode() { tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_mode); }

static void EnableRawMode() {
  tcgetattr(STDIN_FILENO, &old_mode);
  atexit(RestoreOldMode);
  struct termios raw_mode(old_mode);
  raw_mode.c_lflag &= ~(ECHO | ICANON /*| ISIG*/ | IEXTEN);
  raw_mode.c_iflag &= ~(ICRNL | IXON);
  raw_mode.c_oflag &= ~(OPOST);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode);
}

int main() {
  EnableRawMode();
  EPoll ep;
  if (!ep.IsValid()) {
    std::cerr << "epoll is not valid.\n";
    return 1;
  }
  if (!SetNonBlocking(STDIN_FILENO)) {
    std::cerr << "Unable to set stdin nonblocking.\n";
    return 1;
  }
  if (!ep.AddFD(STDIN_FILENO, EPOLLIN | EPOLLERR | EPOLLET)) {
    std::cerr << fmt::format("Can't add {} to epoll.\n", STDIN_FILENO);
    return 1;
  }
  std::vector<struct epoll_event> events;
  events.resize(16);
  bool ok = ep.Wait(&events, -1);
  while (ok) {
    bool stop = false;
    for (auto &event : events) {
      char buffer[256];
      int nread = read(event.data.fd, buffer, GetArrayLength(buffer));
      while (nread > 0) {
        for (int i = 0; i < nread; ++i) {
          if (buffer[i] == 27) {
            stop = true;
            break;
          }
        }
        nread = read(event.data.fd, buffer, GetArrayLength(buffer));
      }
      if (nread < 0) {
        if (errno == EAGAIN) {
          continue;
        } else {
          std::cerr << fmt::format("Read error: {}\n", strerror(errno));
          break;
        }
      }
      if (nread == 0)
        break;
    }
    if (stop)
      break;
    events.resize(16);
    ok = ep.Wait(&events, -1);
  }
  if (!ok) {
    std::cerr << fmt::format("EPoll wait error: {}\n", strerror(errno));
    return 1;
  }
  return 0;
}
