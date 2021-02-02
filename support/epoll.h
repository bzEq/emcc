// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include <sys/epoll.h>

#include <stdint.h>
#include <vector>

namespace emcc {

class EPoll {
public:
  EPoll();
  bool IsValid() { return epfd_ >= 0; };
  bool AddFD(int fd, uint32_t flags);
  bool ModFD(int fd, uint32_t flags);
  bool DelFD(int fd);
  bool Wait(std::vector<struct epoll_event> *events, int timeout);
  ~EPoll();

private:
  int epfd_;
};

} // namespace emcc
