#include "tui/cursor.h"
#include "tui/pixel.h"

#include <gtest/gtest.h>

namespace {
using namespace emcc::tui;

TEST(CursorTest, Distance) {
  EXPECT_TRUE(Cursor::ComputeDistance(80, {0, 0}, {1, 0}) == 80);
  EXPECT_TRUE(Cursor::ComputeDistance(80, {1, 0}, {0, 0}) == -80);
  EXPECT_TRUE(Cursor::ComputeDistance(80, {0, 40}, {1, 20}) == 60);
  EXPECT_TRUE(Cursor::ComputeDistance(80, {1, 20}, {0, 40}) == -60);
  EXPECT_TRUE(Cursor::ComputeDistance(80, {0, 20}, {1, 40}) == 100);
  EXPECT_TRUE(Cursor::ComputeDistance(80, {1, 40}, {0, 20}) == -100);
}

TEST(Cursor, Goto) {
  EXPECT_TRUE(Cursor::Goto(80, {1, 20}, -60) == Cursor(0, 40));
  EXPECT_TRUE(Cursor::Goto(80, {1, 40}, -100) == Cursor(0, 20));
}

TEST(PageTest, PixelTest) {
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

TEST(PageTest, PixelTest1) {
  Pixel p;
  p.set_offset(3, 0);
  EXPECT_TRUE(p.is_head());
  p.set_offset(3, 1);
  EXPECT_TRUE(!p.is_head());
  EXPECT_TRUE(p.offset() == 1);
  p.set_offset(3, 2);
  EXPECT_TRUE(!p.is_head());
  EXPECT_TRUE(p.offset() == 2);
}

} // namespace
