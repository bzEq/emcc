#pragma once

#include "fmt/format.h"
#include "support/result.h"

namespace emcc {

template <class T, std::size_t N>
constexpr inline size_t GetArrayLength(T (&)[N]) {
  return N;
}

inline ResultError<std::string> Err(const char *error) {
  return Err(std::string(error));
}

template <typename... Args>
inline ResultError<std::string> Err(Args &&... args) {
  return ResultError<std::string>{
      .error = std::make_unique<std::string>(
          fmt::format(std::forward<Args>(args)...)),
  };
}

} // namespace emcc
