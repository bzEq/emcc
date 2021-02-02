#pragma once

#include <cassert>
#include <memory>

namespace emcc {

namespace {
template <typename Value>
struct ResultValue {
  std::unique_ptr<Value> value;
};

template <typename Error>
struct ResultError {
  std::unique_ptr<Error> error;
};

template <>
struct ResultValue<void> {};

} // namespace

template <typename Value>
inline ResultValue<Value> Ok(Value &&value) {
  return ResultValue<Value>{
      .value = std::make_unique<Value>(std::forward<Value>(value)),
  };
}

inline ResultValue<void> Ok() { return ResultValue<void>{}; }

template <typename Error>
inline ResultError<Error> Err(Error &&error) {
  return ResultError<Error>{
      .error = std::make_unique<Error>(std::forward<Error>(error)),
  };
}

template <typename Value, typename Error>
class Result {
public:
  Result(Result &&other) = default;

  template <typename V>
  Result(ResultValue<V> &&value)
      : value_(std::move(value.value)), error_(nullptr) {}

  template <typename E>
  Result(ResultError<E> &&error)
      : value_(nullptr), error_(std::move(error.error)) {}

  Value &operator*() {
    assert(value_);
    return *value_;
  }

  Value *operator->() {
    assert(value_);
    return value_.operator->();
  }

  explicit operator bool() const { return value_ != nullptr; }

  const Error &Err() const { return *error_; }

  Error &&Err() { return std::move(*error_); }

private:
  std::unique_ptr<Value> value_;
  std::unique_ptr<Error> error_;
};

template <typename Error>
class Result<void, Error> {
public:
  Result() = default;
  Result(Result &&other) = default;
  Result(ResultValue<void> &&) : error_(nullptr) {}
  template <typename E>
  Result(ResultError<E> &&error) : error_(std::move(error.error)) {}

  explicit operator bool() const { return error_ == nullptr; }

  const Error &Err() const { return *error_; }

  Error &&Err() { return std::move(*error_); }

private:
  std::unique_ptr<Error> error_;
};

} // namespace emcc
