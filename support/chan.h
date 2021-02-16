#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

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

} // namespace emcc
