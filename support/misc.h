#pragma once

#include "fmt/format.h"
#include "support/result.h"

#include <memory>

namespace emcc {

template <typename T>
using Arc = std::shared_ptr<T>;

template <typename T>
using Shared = std::shared_ptr<T>;

template <typename T>
using Owned = std::unique_ptr<T>;

template <class T, std::size_t N>
constexpr inline size_t GetArrayLength(T (&)[N]) {
  return N;
}

inline ResultError<std::string> Err(const char *error) {
  return Err(std::string(error));
}

template <typename... Args>
inline ResultError<std::string> Err(Args &&...args) {
  return ResultError<std::string>{
      .error = std::make_unique<std::string>(
          fmt::format(std::forward<Args>(args)...)),
  };
}

template <typename Num>
inline Num abs_diff(Num a, Num b) {
  return a > b ? a - b : b - a;
}

// Copy from llvm/include/llvm/ADT/iterator_range.h
template <typename IteratorT>
class iterator_range {
  IteratorT begin_iterator, end_iterator;

public:
  // TODO: Add SFINAE to test that the Container's iterators match the range's
  //      iterators.
  template <typename Container>
  iterator_range(Container &&c)
      // TODO: Consider ADL/non-member begin/end calls.
      : begin_iterator(c.begin()), end_iterator(c.end()) {}
  iterator_range(IteratorT begin_iterator, IteratorT end_iterator)
      : begin_iterator(std::move(begin_iterator)),
        end_iterator(std::move(end_iterator)) {}

  IteratorT begin() const { return begin_iterator; }
  IteratorT end() const { return end_iterator; }
  bool empty() const { return begin_iterator == end_iterator; }
};

/// Convenience function for iterating over sub-ranges.
///
/// This provides a bit of syntactic sugar to make using sub-ranges
/// in for loops a bit easier. Analogous to std::make_pair().
template <class T>
iterator_range<T> make_range(T x, T y) {
  return iterator_range<T>(std::move(x), std::move(y));
}

template <typename T>
iterator_range<T> make_range(std::pair<T, T> p) {
  return iterator_range<T>(std::move(p.first), std::move(p.second));
}

} // namespace emcc
