// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "support/epoll.h"

#include <unistd.h>

#include <cassert>
#include <iostream>

namespace emcc {
EPoll::EPoll() : epfd_(-1) {
  epfd_ = epoll_create(1);
  if (epfd_ < 0)
    return;
}

EPoll::~EPoll() {
  if (IsValid())
    ::close(epfd_);
}

bool EPoll::AddFD(int fd, uint32_t flags) {
  struct epoll_event event = {
      .events = flags,
      .data =
          {
              .fd = fd,
          },
  };
  int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event);
  if (ret != 0) {
    return false;
  }
  return true;
}

bool EPoll::ModFD(int fd, uint32_t flags) {
  struct epoll_event event = {
      .events = flags,
      .data =
          {
              .fd = fd,
          },
  };
  int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event);
  if (ret != 0) {
    return false;
  }
  return true;
}

bool EPoll::DelFD(int fd) {
  int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
  if (ret != 0) {
    return false;
  }
  return true;
}

bool EPoll::Wait(std::vector<struct epoll_event> *events, int timeout) {
  assert(events);
  int ret = epoll_wait(epfd_, events->data(), events->size(), timeout);
  if (ret < 0) {
    return false;
  }
  events->resize(ret);
  return true;
}
} // namespace emcc
