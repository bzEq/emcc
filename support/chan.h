#pragma once

#include <errno.h>
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
    return !closed_;
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
    std::unique_lock<std::mutex> l(mu_, std::defer_lock);
    if (!l.try_lock())
      return false;
    if (closed_ || is_full()) {
      l.unlock();
      return false;
    }
    receiver = std::move(chan_[r_]);
    AdvanceRead();
    l.unlock();
    cv_.notify_one();
    return true;
  }

  template <typename E>
  bool put_nowait(E &&e) {
    std::unique_lock<std::mutex> l(mu_, std::defer_lock);
    if (!l.try_lock())
      return false;
    if (closed_ || is_full()) {
      l.unlock();
      return false;
    }
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
        wrapped_(false), chan_(cap) {
    empty_sema_ = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE);
    full_sema_ = eventfd(cap_, EFD_CLOEXEC | EFD_SEMAPHORE);
    use_sema_ = eventfd(1, EFD_CLOEXEC | EFD_SEMAPHORE);
    close_sema_ = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE | EFD_NONBLOCK);
  }

  void close() {
    uint64_t val = ~uint64_t(0) - 1;
    write(close_sema_, &val, sizeof(val));
  }

  bool is_open() const {
    uint64_t val;
    read(close_sema_, &val, sizeof(val));
    return errno == EAGAIN;
  }

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
    err = read(close_sema_, &val, sizeof(val));
    if (errno != EAGAIN)
      return false;
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
    err = read(close_sema_, &val, sizeof(val));
    if (errno != EAGAIN)
      return false;
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
      ::close(empty_sema_);
    if (full_sema_ >= 0)
      ::close(full_sema_);
    if (use_sema_ >= 0)
      ::close(use_sema_);
    if (close_sema_ >= 0)
      ::close(close_sema_);
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
  int empty_sema_, full_sema_, use_sema_, close_sema_;
  size_t r_, w_;
  bool wrapped_;
  std::vector<T> chan_;
};

// FIXME: This impl is not well tested under intensive contention.
template <typename T>
class GoChan {
public:
  explicit GoChan(size_t cap, bool block = true)
      : cap_(cap), r_(0), w_(0), wrapped_(false), closed_(false), chan_(cap_),
        block_(block), get_sema_(-1), put_sema_(-1) {
    get_sema_ = eventfd(0, EFD_CLOEXEC | EFD_SEMAPHORE | EFD_NONBLOCK);
    put_sema_ = eventfd(cap_, EFD_CLOEXEC | EFD_SEMAPHORE | EFD_NONBLOCK);
  }

  int receive_chan() const { return get_sema_; }

  int send_chan() const { return put_sema_; }

  bool get(T &receiver) {
    std::unique_lock<std::mutex> l(mu_);
    if (block_)
      cv_.wait(l, [this] { return closed_ || !empty_nolock(); });
    if (closed_ || empty_nolock())
      return false;
    uint64_t val = 0;
    receiver = std::move(chan_[r_]);
    AdvanceRead();
    read(get_sema_, &val, sizeof(val));
    assert(val == 1);
    write(put_sema_, &val, sizeof(val));
    l.unlock();
    cv_.notify_one();
    return true;
  }

  template <typename... Args>
  bool put(Args &&...args) {
    std::unique_lock<std::mutex> l(mu_);
    if (block_)
      cv_.wait(l, [this] { return closed_ || !is_full_nolock(); });
    if (closed_ || is_full_nolock())
      return false;
    chan_.emplace(chan_.begin() + w_, std::forward<Args>(args)...);
    AdvanceWrite();
    uint64_t val = 0;
    read(put_sema_, &val, sizeof(val));
    assert(val == 1);
    write(get_sema_, &val, sizeof(val));
    l.unlock();
    cv_.notify_one();
    return true;
  }

  size_t size() const {
    std::unique_lock<std::mutex> l(mu_);
    return size_nolock();
  }

  bool empty() const {
    std::unique_lock<std::mutex> l(mu_);
    return empty_nolock();
  }

  bool is_full() const {
    std::unique_lock<std::mutex> l(mu_);
    return is_full_nolock();
  }

  void close() {
    std::unique_lock<std::mutex> l(mu_);
    closed_ = true;
    if (put_sema_ >= 0) {
      ::close(put_sema_);
      put_sema_ = -1;
    }
    if (get_sema_ >= 0) {
      ::close(get_sema_);
      put_sema_ = -1;
    }
  }

  ~GoChan() { close(); }

private:
  size_t size_nolock() const {
    if (!wrapped_ && r_ == w_)
      return 0;
    if (r_ == w_)
      return cap_;
    if (w_ > r_)
      return w_ - r_;
    return w_ + cap_ - r_;
  }

  bool empty_nolock() const { return size_nolock() == 0; }

  bool is_full_nolock() const { return size_nolock() == cap_; }

  bool is_open_nolock() const {
    return !closed_ && get_sema_ >= 0 && put_sema_ >= 0;
  }

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
  size_t r_, w_;
  bool wrapped_, closed_;
  mutable std::mutex mu_;
  std::condition_variable cv_;
  std::vector<T> chan_;
  bool block_;
  int get_sema_, put_sema_;
};

} // namespace emcc
