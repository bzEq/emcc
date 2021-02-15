#include "support/prefix_sum.h"
#include "support/random.h"

#include <gtest/gtest.h>

namespace {

using namespace emcc;

TEST(PrefixSumTest, InsertTest) {
  PrefixSum<int> s;
  s.Insert(0, 0);
  EXPECT_TRUE(s.size() == 1);
  s.Insert(1, 1);
  EXPECT_TRUE(s.size() == 2);
}

TEST(PrefixSumTest, OneToN) {
  PrefixSum<int> s;
  const int N = 2;
  for (int i = 0; i <= N; ++i)
    s.Insert(i, i);
  EXPECT_TRUE(s.size() == N + 1);
  EXPECT_TRUE(s.At(0) == 0);
  EXPECT_TRUE(s.At(1) == 1);
  for (int i = 0; i <= N; ++i)
    EXPECT_TRUE(s.At(i) == i);
  EXPECT_TRUE(s.size() == N + 1);
  EXPECT_TRUE(s.GetPrefixSum(0) == 0);
  for (int i = 0; i <= N; ++i)
    EXPECT_TRUE(s.GetPrefixSum(i) == i * (i + 1) / 2);
}

TEST(PrefixSumTest, OneToN1) {
  PrefixSum<int> s;
  for (int i = 0; i <= 100; ++i)
    s.Insert(i, i);
  EXPECT_TRUE(s.size() == 101);
  for (int i = 0; i <= 100; ++i)
    EXPECT_TRUE(s.At(i) == i);
  EXPECT_TRUE(s.size() == 101);
  EXPECT_TRUE(s.GetPrefixSum(100) == 5050);
  for (int i = 0; i <= 100; ++i)
    s.Add(i, i);
  EXPECT_TRUE(s.GetPrefixSum(100) == 2 * 5050);
}

TEST(PrefixSumTest, OneToN2) {
  PrefixSum<int> s;
  for (int i = 0; i <= 100; ++i)
    s.Insert(i, i);
  EXPECT_TRUE(s.size() == 101);
  for (int i = 0; i <= 100; ++i)
    EXPECT_TRUE(s.At(i) == i);
  EXPECT_TRUE(s.size() == 101);
  EXPECT_TRUE(s.GetPrefixSum(100) == 5050);
  for (int i = 0; i <= 100; ++i)
    s.Add(i, -i);
  EXPECT_TRUE(s.GetPrefixSum(100) == 0);
}

TEST(PrefixSumTest, EraseTest) {
  PrefixSum<int> s;
  for (int i = 0; i <= 100; ++i)
    s.Insert(i, i);
  EXPECT_TRUE(s.size() == 101);
  for (int i = 100; i >= 0; --i)
    EXPECT_TRUE(s.Remove(i));
  EXPECT_TRUE(s.size() == 0);
}

TEST(PrefixSumTest, EraseTest1) {
  PrefixSum<int> s;
  for (int i = 0; i <= 100; ++i)
    s.Insert(i, i);
  EXPECT_TRUE(s.size() == 101);
  for (int i = 0; i <= 100; ++i)
    EXPECT_TRUE(s.Remove(0));
}

TEST(PrefixSumTest, SearchTest) {
  PrefixSum<int> s;
  for (int i = 0; i <= 100; ++i)
    s.Insert(i, i);
  EXPECT_TRUE(s.LowerBound(6) == 3);
  EXPECT_TRUE(s.LowerBound(7) == 4);
}

TEST(PrefixSumTest, SearchTest1) {
  PrefixSum<int> s;
  for (int i = 0; i <= 100; ++i)
    s.Insert(i, i);
  EXPECT_TRUE(s.UpperBound(6) == 4);
  EXPECT_TRUE(s.UpperBound(7) == 4);
}

TEST(PrefixSumTest, Benchmark) {
  PrefixSum<int> s;
  Random rnd(std::time(nullptr));
  const size_t N = 120 * 40;
  for (size_t i = 0; i < N; ++i)
    s.Insert(i, 0);
  for (size_t i = 0; i < N; ++i) {
    size_t n = rnd.Next() * s.size();
    s.At(n);
  }
}

TEST(PrefixSumTest, Benchmark1) {
  PrefixSum<int> s;
  Random rnd(std::time(nullptr));
  const size_t N = 1 << 16;
  for (size_t i = 0; i < N; ++i)
    s.Insert(i, 0);
  for (size_t i = 0; i < N; ++i) {
    size_t n = rnd.Next() * s.size();
    s.At(n);
  }
  s.At(s.size() / 2);
  // std::cout << s.height() << std::endl;
}

} // namespace
