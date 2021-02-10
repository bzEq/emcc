#include "support/prefix_sum.h"

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
  for (int i = 0; i <= 100; ++i)
    s.Insert(i, i);
  EXPECT_TRUE(s.size() == 101);
  for (int i = 0; i <= 100; ++i)
    EXPECT_TRUE(s.At(i) == i);
  EXPECT_TRUE(s.size() == 101);
  for (int i = 0; i <= 100; ++i)
    EXPECT_TRUE(s.GetPrefixSum(i) == i * (i+1) / 2);
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

} // namespace
