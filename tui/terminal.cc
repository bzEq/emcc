#include "tui/terminal.h"
#include "support/epoll.h"
#include "support/sys.h"

namespace emcc::tui {

void Terminal::DisplayErrorMessage(const std::string &msg) { return; }

bool Terminal::HaveToStop() { return false; }

static bool FillBuffer(int fd, std::string &buffer) {
  char tmp[4096];
  int nread = ::read(fd, tmp, GetArrayLength(tmp));
  while (nread > 0) {
    buffer.append(tmp, nread);
    nread = ::read(fd, tmp, GetArrayLength(tmp));
  }
  if (nread < 0 && errno != EAGAIN)
    return false;
  return true;
}

void Terminal::LoopInput() {
  EPoll monitor;
  if (!monitor.is_open()) {
    DisplayErrorMessage("Can't open event monitor, exiting...");
    return;
  }
  SetNonBlocking(input_fd_);
  monitor.AddFD(input_fd_, EPOLLIN | EPOLLERR | EPOLLET);
  std::vector<struct epoll_event> events;
  events.resize(1);
  while (!HaveToStop()) {
    bool ok = monitor.Wait(&events, -1);
    if (!ok) {
      DisplayErrorMessage("Failed getting events from monitor");
      break;
    }
    assert(events.size() <= 1);
    if (events.empty()) {
      events.resize(1);
      continue;
    }
    struct epoll_event happened = events.back();
    if (happened.data.fd != input_fd_)
      continue;
    std::string buffer;
    if (!FillBuffer(happened.data.fd, buffer)) {
      DisplayErrorMessage(
          fmt::format("Failed reading data from %d", happened.data.fd));
      break;
    }
    for (auto c : buffer) {
      input_receiver_->put(c);
    }
  }
}

} // namespace emcc
