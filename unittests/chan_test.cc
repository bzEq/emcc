#include "support/chan.h"

#include <gtest/gtest.h>
#include <thread>

namespace {

using namespace emcc;

TEST(ChanTest, Basic) {
  Chan<int, 4> c;
  for (int i = 0; i < 4; ++i)
    c.Push(i);
  EXPECT_TRUE(c.size() == 4);
  for (int i = 0; i < 4; ++i)
    EXPECT_TRUE(c.Pop() == i);
  EXPECT_TRUE(c.empty());
}

TEST(ChanTest, Wrap) {
  Chan<int, 4> c;
  c.Push(0);
  c.Push(1);
  c.Push(2);
  c.Push(3);
  EXPECT_TRUE(c.Pop() == 0);
  EXPECT_TRUE(c.Pop() == 1);
  c.Push(4);
  EXPECT_TRUE(c.size() == 3);
  c.Push(5);
  EXPECT_TRUE(c.size() == 4);
  EXPECT_TRUE(c.Pop() == 2);
  EXPECT_TRUE(c.Pop() == 3);
  EXPECT_TRUE(c.Pop() == 4);
  EXPECT_TRUE(c.Pop() == 5);
}

TEST(ChanTest, WaitForRead) {
  Chan<int, 4> c;
  for (int i = 0; i < 4; ++i)
    c.Push(i);
  auto t = std::thread([&] { EXPECT_TRUE(c.Pop() == 0); });
  c.Push(4);
  t.join();
  EXPECT_TRUE(c.size() == 4);
}

TEST(ChanTest, WaitForWrite) {
  Chan<int, 4> c;
  auto t = std::thread([&] { c.Push(1024); });
  EXPECT_TRUE(c.Pop() == 1024);
  t.join();
}

TEST(ChanBenchmark, ReadWrite) {
  const size_t Num = 1 << 22;
  const size_t Cap = 1 << 10;
  Chan<int, Cap> c;
  auto a = std::thread([&] {
    for (int i = 0; i < Num; ++i)
      c.Pop();
  });
  auto b = std::thread([&] {
    for (int i = 0; i < Num; ++i)
      c.Push(i);
  });
  a.join();
  b.join();
  EXPECT_TRUE(c.empty());
}

} // namespace
