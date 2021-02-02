#pragma once

#include <algorithm>
#include <optional>
#include <vector>

namespace emcc {

template <typename T>
class SizeLimitQueue {
  const size_t max_size_;
  std::vector<T> queue_;
  size_t r_, w_;
  bool wrapped_;

  void AdvanceRead() {
    if (r_ + 1 == max_size_) {
      wrapped_ = !wrapped_;
      r_ = 0;
    } else {
      ++r_;
    }
  }

  void AdvanceWrite() {
    if (w_ + 1 == max_size_) {
      wrapped_ = !wrapped_;
      w_ = 0;
    } else {
      ++w_;
    }
  }

public:
  SizeLimitQueue(size_t max_size)
      : max_size_(std::max(1UL, max_size)), queue_(max_size_), r_(0), w_(0),
        wrapped_(false) {}

  bool empty() const { return !wrapped_ && r_ == w_; }

  size_t size() const {
    if (empty())
      return 0;
    if (r_ == w_)
      return max_size_;
    if (w_ > r_)
      return w_ - r_;
    return w_ + max_size_ - r_;
  }

  template <typename E>
  void PushBack(E &&e) {
    if (wrapped_ && r_ == w_)
      AdvanceRead();
    queue_[w_] = std::forward<E>(e);
    AdvanceWrite();
  }

  std::optional<T> Pop() {
    if (empty())
      return std::nullopt;
    T result(std::move(queue_[r_]));
    AdvanceRead();
    return std::move(result);
  }

  std::optional<T> PopBack() {
    if (empty())
      return std::nullopt;
    if (w_ == 0) {
      w_ = max_size_ - 1;
      wrapped_ = !wrapped_;
    } else {
      --w_;
    }
    T result(std::move(queue_[w_]));
    return std::move(result);
  }
};

} // namespace emcc
