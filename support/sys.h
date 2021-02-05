// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "fmt/format.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <assert.h>
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

  static std::unique_ptr<MemoryBuffer> Create(const std::string &filename,
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

class MMapFile {
public:
  static constexpr size_t kDefaultBlockSize = 8UL << 20;

  struct Block {
    char *data;
    size_t length;
    Block() : data(nullptr), length(0) {}

    bool operator==(Block &other) {
      return data == other.data && length == other.length;
    }

    bool operator!=(Block &other) { return !(*this == other); }

    void reset() {
      data = nullptr;
      length = 0;
    }
  };

  class iterator {
  public:
    iterator &operator++() {
      assert(block_.data);
      ::munmap(block_.data, block_.length);
      offset_ += block_.length;
      return Advance();
    }

    Block &operator*() { return block_; }

    Block *operator->() { return &block_; }

    bool operator==(iterator &other) { return block_ == other.block_; }

    bool operator!=(iterator &other) { return block_ != other.block_; }

  private:
    friend class MMapFile;
    iterator(int fd, size_t total_length, Block &block)
        : fd_(fd), total_length_(total_length), offset_(0), block_(block) {}

    iterator &Advance() {
      if (offset_ >= total_length_) {
        block_.reset();
      } else {
        if (offset_ + block_.length > total_length_)
          block_.length = total_length_ - offset_;
        block_.data =
            (char *)::mmap(nullptr, block_.length, PROT_READ | PROT_WRITE,
                           MAP_PRIVATE, fd_, offset_);
        if (block_.data == MAP_FAILED)
          block_.reset();
      }
      return *this;
    }

    int fd_;
    const size_t total_length_;
    off_t offset_;
    Block &block_;
  };

  MMapFile(const std::string &filename, size_t block_size = kDefaultBlockSize)
      : block_size_(block_size), fd_(-1), length_(0) {
    fd_ = ::open(filename.c_str(), 0644, O_RDWR);
    if (fd_ < 0)
      return;
    struct stat statbuf;
    int err = ::fstat(fd_, &statbuf);
    if (err < 0)
      return;
    length_ = statbuf.st_size;
  }

  iterator begin() {
    assert(block_size_ % sysconf(_SC_PAGE_SIZE) == 0);
    block_.length = block_size_;
    iterator it(fd_, length_, block_);
    return it.Advance();
  }

  iterator end() { return iterator(fd_, length_, end_block_); }

  bool IsOpen() const { return fd_ >= 0; }

  size_t length() const { return length_; }

  ~MMapFile() {
    if (block_.data)
      munmap(block_.data, block_.length);
    if (fd_ >= 0)
      ::close(fd_);
  }

private:
  const size_t block_size_;
  int fd_;
  size_t length_;
  Block block_, end_block_;
};

template <typename... Args>
inline void Report(const char *prefix, Args &&...args) {
  std::cerr << fmt::format("{}{}", prefix, std::forward<Args>(args)...) << "\n";
}

template <typename... Args>
inline void Die(Args &&...args) {
  Report("fatal: ", std::forward<Args>(args)...);
  exit(1);
}

} // namespace emcc
