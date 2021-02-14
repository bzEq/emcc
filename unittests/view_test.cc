#include "tui/basis.h"
#include "tui/view.h"

#include <gtest/gtest.h>

namespace {
using namespace emcc::tui;

TEST(ViewTest, Distance) {
  EXPECT_TRUE(WrapDistance(80, {0, 0}, {1, 0}) == 80);
  EXPECT_TRUE(WrapDistance(80, {1, 0}, {0, 0}) == -80);
  EXPECT_TRUE(WrapDistance(80, {0, 40}, {1, 20}) == 60);
  EXPECT_TRUE(WrapDistance(80, {1, 20}, {0, 40}) == -60);
  EXPECT_TRUE(WrapDistance(80, {0, 20}, {1, 40}) == 100);
  EXPECT_TRUE(WrapDistance(80, {1, 40}, {0, 20}) == -100);
}

TEST(ViewTest, Jump) {
  EXPECT_TRUE(JumpTo(80, {1, 20}, -60) == Cursor(0, 40));
  EXPECT_TRUE(JumpTo(80, {1, 40}, -100) == Cursor(0, 20));
}

TEST(ViewTest, PixelTest) {
  auto vec = Pixel::MakeSeries(0, 16);
  EXPECT_TRUE(vec.size() == 16);
  EXPECT_TRUE(vec.front().is_head());
  EXPECT_TRUE(vec.front().length() == 16);
  for (size_t i = 1; i < vec.size(); ++i) {
    EXPECT_TRUE(!vec[i].is_head());
  }
  for (size_t i = 1; i < vec.size(); ++i) {
    EXPECT_TRUE(vec[i].offset() == i);
  }
}

} // namespace
