// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include <random>

namespace emcc {

class Random {
public:
  Random() : distribution_(0, 1) {}

  explicit Random(int64_t seed) : generator_(seed), distribution_(0, 1) {}

  float Next() { return distribution_(generator_); }

  int NextInt() { return int_distribution_(generator_); }

private:
  std::mt19937_64 generator_;
  std::uniform_real_distribution<float> distribution_;
  std::uniform_int_distribution<int> int_distribution_;
};

} // namespace emcc
