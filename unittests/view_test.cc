#include "tui/view.h"

#include <gtest/gtest.h>

namespace {
using namespace emcc::tui;

TEST(ViewTest, Distance) {
  EXPECT_TRUE(WrapDistance(80, {0, 0}, {0, 1}) == 80);
  EXPECT_TRUE(WrapDistance(80, {0, 1}, {0, 0}) == -80);
  EXPECT_TRUE(WrapDistance(80, {40, 0}, {20, 1}) == 60);
  EXPECT_TRUE(WrapDistance(80, {20, 1}, {40, 0}) == -60);
  EXPECT_TRUE(WrapDistance(80, {20, 0}, {40, 1}) == 100);
  EXPECT_TRUE(WrapDistance(80, {40, 1}, {20, 0}) == -100);
}

TEST(ViewTest, Jump) {
  EXPECT_TRUE(JumpTo(80, {20, 1}, -60) == Cursor(40, 0));
  EXPECT_TRUE(JumpTo(80, {40, 1}, -100) == Cursor(20, 0));
}

TEST(ViewTest, ResetViewPort) {
  Viewport v(120, 40);
  const size_t N = 1UL << 10;
  for (size_t i = 0; i < N; ++i)
    v.Reset();
}

} // namespace
