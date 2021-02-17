// Copyright (c) 2020 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "support/random.h"
#include "support/rope.h"

#include <ext/rope>
#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <time.h>

namespace {

using Rope = emcc::Rope<char, 1UL << 12>;

TEST(RopeBenchmark, ExtRopeRandomInsert) {
  emcc::Random rnd(std::time(nullptr));
  __gnu_cxx::rope<char> s;
  for (size_t i = 0; i < (1 << 20); ++i) {
    s.insert(rnd.NextInt() % (s.size() + 1), '0');
  }
}

TEST(RopeBenchmark, RopeRandomInsert) {
  emcc::Random rnd(std::time(nullptr));
  Rope rope;
  for (size_t i = 0; i < (1 << 20); ++i) {
    rope.Insert(rnd.NextInt() % (rope.size() + 1), '0');
  }
  std::cout << "Rope height: " << rope.height() << std::endl;
  std::cout << "Rope nodes: " << rope.nodes() << std::endl;
}

TEST(RopeBenchmark, StringRandomInsert) {
  emcc::Random rnd(std::time(nullptr));
  std::string s;
  for (size_t i = 0; i < (1 << 20); ++i) {
    s.insert(s.begin() + (rnd.NextInt() % (s.size() + 1)), '0');
  }
}

TEST(RopeBenchmark, ExtRopeAppend) {
  __gnu_cxx::rope<char> s;
  for (size_t i = 0; i < (1 << 25); ++i)
    s.push_back('0');
}

TEST(RopeBenchmark, RopeAppend) {
  Rope rope;
  for (size_t i = 0; i < (1 << 25); ++i)
    rope.Append('0');
}

TEST(RopeBenchmark, StringAppend) {
  std::string s;
  for (size_t i = 0; i < (1 << 25); ++i)
    s.push_back('0');
}

TEST(RopeBenchmark, SmallPiece) {
  emcc::Rope<int, 1UL> s;
  emcc::Random rnd(std::time(nullptr));
  const size_t N = 1 << 16;
  for (size_t i = 0; i < N; ++i)
    s.Insert(i, 0);
  for (size_t i = 0; i < N; ++i) {
    size_t n = rnd.Next() * s.size();
    s.At(n);
  }
}

TEST(RopeBenchmark, RopeAppendAndRandomInsert) {
  emcc::Random rnd(std::time(nullptr));
  Rope rope;
  for (size_t i = 0; i < 10000000; ++i) {
    rope.Append('0');
  }
  for (size_t i = 0; i < 100000; ++i) {
    rope.At(rnd.NextInt() % rope.size());
  }
  std::cout << "Rope height: " << rope.height() << std::endl;
}

TEST(RopeBenchmark, CountNodes) {
  std::string s(15UL << 20, '0');
  Rope rope;
  rope.Append(s.data(), s.size());
  std::cout << "Rope nodes: " << rope.nodes() << std::endl;
  std::cout << "Rope meta size: " << rope.meta_size() << std::endl;
}

} // namespace
