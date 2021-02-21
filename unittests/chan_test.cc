#include "support/chan.h"
#include "support/epoll.h"

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
  t.join();
  c.put(4);
  EXPECT_TRUE(c.size() == 4);
}

TEST(SemaChanTest, WaitForRead) {
  SemaChan<int> c(4);
  for (int i = 0; i < 4; ++i)
    c.put(i);
  auto t = std::thread([&] {
    int res = -1;
    EXPECT_TRUE(c.get(res));
    EXPECT_TRUE(res == 0);
  });
  t.join();
  c.put(4);
  EXPECT_TRUE(c.size() == 4);
}

TEST(ChanTest, WaitForWrite) {
  Chan<int, 4> c;
  auto t = std::thread([&] { c.put(1024); });
  int res;
  EXPECT_TRUE(c.get(res) && res == 1024);
  t.join();
}

TEST(SemaChanTest, WaitForWrite) {
  SemaChan<int> c(4);
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

TEST(SemaChanBenchmark, ReadWrite) {
  const size_t Num = 1 << 16;
  const size_t Cap = 1 << 8;
  SemaChan<int> c(Cap);
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

TEST(GoChanBenchmark, ReadWrite) {
  const size_t Num = 1 << 16;
  const size_t Cap = 1 << 8;
  GoChan<int> c(Cap);
  auto a = std::thread([&] {
    EPoll ep;
    ep.AddFD(c.receive_chan(), EPOLLIN);
    std::vector<epoll_event> events(1);
    for (int i = 0; i < Num; ++i) {
      int res;
      EXPECT_TRUE(ep.Wait(&events, -1));
      EXPECT_TRUE(!events.empty());
      EXPECT_TRUE(events.back().data.fd == c.receive_chan());
      EXPECT_TRUE(c.get_nowait(res));
    }
  });
  auto b = std::thread([&] {
    EPoll ep;
    ep.AddFD(c.send_chan(), EPOLLIN);
    std::vector<epoll_event> events(1);
    for (int i = 0; i < Num; ++i) {
      EXPECT_TRUE(ep.Wait(&events, -1));
      EXPECT_TRUE(!events.empty());
      EXPECT_TRUE(events.back().data.fd == c.send_chan());
      EXPECT_TRUE(c.put_nowait(i));
    }
  });
  a.join();
  b.join();
  EXPECT_TRUE(c.empty());
}

} // namespace
