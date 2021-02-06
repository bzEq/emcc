#include "support/chan.h"

#include <gtest/gtest.h>
#include <thread>

namespace {

using namespace emcc;

TEST(ChanTest, Basic) {
  Chan<int, 4> c;
  for (int i = 0; i < 4; ++i)
    c.put(i);
  EXPECT_TRUE(c.size() == 4);
  for (int i = 0; i < 4; ++i) {
    int v;
    EXPECT_TRUE(c.get(v));
    EXPECT_TRUE(v == i);
  }
  EXPECT_TRUE(c.empty());
}

TEST(ChanTest, Wrap) {
  Chan<int, 4> c;
  c.put(0);
  c.put(1);
  c.put(2);
  c.put(3);
  int res;
  EXPECT_TRUE(c.get(res) && res == 0);
  EXPECT_TRUE(c.get(res) && res == 1);
  c.put(4);
  EXPECT_TRUE(c.size() == 3);
  c.put(5);
  EXPECT_TRUE(c.size() == 4);
  EXPECT_TRUE(c.get(res) && res == 2);
  EXPECT_TRUE(c.get(res) && res == 3);
  EXPECT_TRUE(c.get(res) && res == 4);
  EXPECT_TRUE(c.get(res) && res == 5);
}

TEST(ChanTest, WaitForRead) {
  Chan<int, 4> c;
  for (int i = 0; i < 4; ++i)
    c.put(i);
  auto t = std::thread([&] {
    int res;
    EXPECT_TRUE(c.get(res) && res == 0);
  });
  c.put(4);
  t.join();
  EXPECT_TRUE(c.size() == 4);
}

TEST(ChanTest, WaitForWrite) {
  Chan<int, 4> c;
  auto t = std::thread([&] { c.put(1024); });
  int res;
  EXPECT_TRUE(c.get(res) && res == 1024);
  t.join();
}

TEST(ChanBenchmark, ReadWrite) {
  const size_t Num = 1 << 22;
  const size_t Cap = 1 << 10;
  Chan<int, Cap> c;
  auto a = std::thread([&] {
    for (int i = 0; i < Num; ++i) {
      int res;
      c.get(res);
    }
  });
  auto b = std::thread([&] {
    for (int i = 0; i < Num; ++i)
      c.put(i);
  });
  a.join();
  b.join();
  EXPECT_TRUE(c.empty());
}

} // namespace
