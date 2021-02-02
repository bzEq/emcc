#pragma once

#include <condition_variable>
#include <mutex>

namespace emcc {

template <typename T, size_t Cap>
class Chan {
private:
  T chan_[Cap];
  size_t r_, w_;
  bool wrapped_;

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
  Chan() : r_(0), w_(0), wrapped_(false) {}

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

  T Pop() {
    std::unique_lock<std::mutex> l(mu_);
    cv_.wait(l, [this] { return !empty(); });
    T result(std::move(chan_[r_]));
    AdvanceRead();
    l.unlock();
    cv_.notify_one();
    return result;
  }

  template<typename E>
  void Push(E &&e) {
    std::unique_lock<std::mutex> l(mu_);
    cv_.wait(l, [this] { return size() != Cap; });
    chan_[w_] = std::forward<E>(e);
    AdvanceWrite();
    l.unlock();
    cv_.notify_one();
  }
};

template <typename T>
class Chan<T, 0> {};

} // namespace emcc
