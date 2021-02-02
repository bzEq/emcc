// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "fmt/format.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <functional>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace emcc {

class MemoryBuffer {
private:
  MemoryBuffer(const std::string &filename)
      : filename_(filename), fd_(-1), buffer_(nullptr), length_(0) {
    fd_ = ::open(filename.c_str(), 0644, O_RDWR);
    if (fd_ < 0)
      return;
    struct stat statbuf;
    int err = ::fstat(fd_, &statbuf);
    if (err < 0)
      return;
    length_ = statbuf.st_size;
    buffer_ = (char *)::mmap(nullptr, length_, PROT_READ | PROT_WRITE,
                             MAP_PRIVATE, fd_, 0);
    if (buffer_ == MAP_FAILED)
      buffer_ = nullptr;
  }

  MemoryBuffer(const std::string &filename, size_t length)
      : filename_(filename), fd_(-1), buffer_(nullptr), length_(length) {
    fd_ = ::open(filename.c_str(), 0644, O_CREAT | O_TRUNC | O_RDWR);
    if (fd_ < 0)
      return;
    buffer_ = (char *)::mmap(nullptr, length_, PROT_READ | PROT_WRITE,
                             MAP_PRIVATE, fd_, 0);
    if (buffer_ == MAP_FAILED)
      buffer_ = nullptr;
  }

public:
  static std::unique_ptr<MemoryBuffer>
  OpenIfExists(const std::string &filename) {
    auto membuf = std::unique_ptr<MemoryBuffer>(new MemoryBuffer(filename));
    if (!membuf->buffer())
      return nullptr;
    return membuf;
  }

  static std::unique_ptr<MemoryBuffer> Truncate(const std::string &filename,
                                                size_t length) {
    auto membuf =
        std::unique_ptr<MemoryBuffer>(new MemoryBuffer(filename, length));
    if (!membuf->buffer())
      return nullptr;
    return membuf;
  }

  const std::string &filename() const { return filename_; }

  const char *buffer() const { return buffer_; }

  char *buffer() { return buffer_; }

  const char *slice(size_t offset) const { return buffer_ + offset; }

  char *slice(size_t offset) { return buffer_ + offset; }

  size_t Write(size_t offset, const char *src, size_t length) {
    if (offset >= length_)
      return 0;
    size_t bytes_to_write = std::min(length_ - offset, length);
    memmove(buffer_ + offset, src, bytes_to_write);
    return bytes_to_write;
  }

  bool Write(size_t offset, char c) {
    if (offset >= length_)
      return false;
    buffer_[offset] = c;
    return true;
  }

  size_t length() const { return length_; }

  ~MemoryBuffer() {
    if (buffer_) {
      ::munmap(buffer_, length_);
    }
    if (fd_ >= 0) {
      ::close(fd_);
    }
  }

private:
  std::string filename_;
  int fd_;
  char *buffer_;
  size_t length_;
};

template <typename... Args>
inline void Report(const char *prefix, Args &&... args) {
  std::cerr << fmt::format("{}{}", prefix, std::forward<Args>(args)...) << "\n";
}

template <typename... Args>
inline void Die(Args &&... args) {
  Report("fatal: ", std::forward<Args>(args)...);
  exit(1);
}

} // namespace emcc
