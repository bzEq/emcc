#pragma once

#include <sys/eventfd.h>
#include <unistd.h>

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>
#include <vector>

namespace emcc {

template <typename T, size_t Cap = 16>
class Chan {
private:
  T chan_[Cap];
  size_t r_, w_;
  bool wrapped_, closed_;

  std::mutex mu_;
  std::condition_variable cv_;

  void AdvanceRead() {
    if (r_ + 1 == Cap) {
      wrapped_ = !wrapped_;
      r_ = 0;
    } else {
      ++r_;
    }
  }

  void AdvanceWrite() {
    if (w_ + 1 == Cap) {
      wrapped_ = !wrapped_;
      w_ = 0;
    } else {
      ++w_;
    }
  }

public:
  Chan() : r_(0), w_(0), wrapped_(false), closed_(false) {}

  bool empty() const { return !wrapped_ && r_ == w_; }

  size_t size() const {
    if (empty())
      return 0;
    if (r_ == w_)
      return Cap;
    if (w_ > r_)
      return w_ - r_;
    return w_ + Cap - r_;
  }

  bool is_full() const { return size() == Cap; }

  bool is_open() {
    std::unique_lock<std::mutex> l(mu_);
    return closed_;
  }

  void close() {
    std::unique_lock<std::mutex> l(mu_);
    closed_ = true;
  }

  bool get(T &receiver) {
    std::unique_lock<std::mutex> l(mu_);
    cv_.wait(l, [this] { return closed_ || !empty(); });
    if (closed_)
      return false;
    receiver = std::move(chan_[r_]);
    AdvanceRead();
    l.unlock();
    cv_.notify_one();
    return true;
  }

  bool get_nowait(T &receiver) {
    std::unique_lock<std::mutex> l(mu_);
    if (closed_ || is_full())
      return false;
    receiver = std::move(chan_[r_]);
    AdvanceRead();
    l.unlock();
    cv_.notify_one();
    return true;
  }

  template <typename E>
  bool put_nowait(E &&e) {
    std::unique_lock<std::mutex> l(mu_);
    if (closed_ || is_full())
      return false;
    chan_[w_] = std::forward<E>(e);
    AdvanceWrite();
    l.unlock();
    cv_.notify_one();
    return true;
  }

  template <typename E>
  bool put(E &&e) {
    std::unique_lock<std::mutex> l(mu_);
    cv_.wait(l, [this] { return closed_ || !is_full(); });
    if (closed_)
      return false;
    chan_[w_] = std::forward<E>(e);
    AdvanceWrite();
    l.unlock();
    cv_.notify_one();
    return true;
  }
};

template <typename T>
class Chan<T, 0> {};

template <typename T>
class SemaChan {
public:
  explicit SemaChan(size_t cap)
      : cap_(cap), empty_sema_(-1), full_sema_(-1), use_sema_(-1), r_(0), w_(0),
        wrapped_(false), closed_(false), chan_(cap) {
    empty_sema_ = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
    full_sema_ = eventfd(cap_, EFD_CLOEXEC | EFD_SEMAPHORE);
    use_sema_ = eventfd(1, EFD_CLOEXEC | EFD_SEMAPHORE);
  }

  bool is_open() const { return !closed_; }

  bool empty() const { return !wrapped_ && r_ == w_; }

  size_t size() const {
    if (empty())
      return 0;
    if (r_ == w_)
      return cap_;
    if (w_ > r_)
      return w_ - r_;
    return w_ + cap_ - r_;
  }

  bool is_full() const { return size() == cap_; }

  template <typename E>
  bool put(E &&e) {
    uint64_t val;
    int err;
    err = read(full_sema_, &val, sizeof(val));
    assert(err > 0);
    err = read(use_sema_, &val, sizeof(val));
    assert(err > 0);
    chan_.emplace(chan_.begin() + w_, std::forward<E>(e));
    AdvanceWrite();
    val = 1;
    err = write(use_sema_, &val, sizeof(val));
    assert(err > 0);
    err = write(empty_sema_, &val, sizeof(val));
    assert(err > 0);
    return true;
  }

  bool get(T &receiver) {
    uint64_t val;
    int err;
    err = read(empty_sema_, &val, sizeof(val));
    assert(err > 0);
    err = read(use_sema_, &val, sizeof(val));
    assert(err > 0);
    receiver = std::move(chan_[r_]);
    AdvanceRead();
    err = write(use_sema_, &val, sizeof(val));
    assert(err > 0);
    err = write(full_sema_, &val, sizeof(val));
    assert(err > 0);
    return true;
  }

  ~SemaChan() {
    if (empty_sema_ >= 0)
      close(empty_sema_);
    if (full_sema_ >= 0)
      close(full_sema_);
    if (use_sema_ >= 0)
      close(use_sema_);
  }

private:
  void AdvanceRead() {
    if (r_ + 1 == cap_) {
      wrapped_ = !wrapped_;
      r_ = 0;
    } else {
      ++r_;
    }
  }

  void AdvanceWrite() {
    if (w_ + 1 == cap_) {
      wrapped_ = !wrapped_;
      w_ = 0;
    } else {
      ++w_;
    }
  }

  const size_t cap_;
  int empty_sema_, full_sema_, use_sema_;
  size_t r_, w_;
  bool wrapped_, closed_;
  std::vector<T> chan_;
};

} // namespace emcc
