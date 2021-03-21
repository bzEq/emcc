#include "editor/mono_buffer.h"
#include "support/misc.h"
#include "tui/cursor.h"

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

TEST(RegionTest, SizeTest) {
  Region region(8, {1, 7}, {2, 7});
  EXPECT_TRUE(region.size() == 8);
}

TEST(RegionTest, SizeTest1) {
  Region region(8, {4, 7}, {4, 7});
  EXPECT_TRUE(region.size() == 0);
}

TEST(RegionTest, SizeTest3) {
  Region region(8, {4, 7}, {16, 7});
  EXPECT_TRUE(region.size() == 96);
}

TEST(RegionTest, IteratorTest) {
  Region region(8, {4, 7}, {16, 7});
  Cursor ref(4, 7);
  size_t s = 0;
  for (auto c : region) {
    ++s;
    EXPECT_TRUE(ref == c);
    ref = Cursor::Goto(8, ref, 1);
  }
  EXPECT_TRUE(ref.y == 16);
  EXPECT_TRUE(ref.x == 7);
  EXPECT_TRUE(s == 96);
}

TEST(RegionTest, ContainTest) {
  Region region(8, {4, 0}, {16, 0});
  EXPECT_TRUE(region.contains({4, 0}));
  EXPECT_TRUE(!region.contains({16, 0}));
  EXPECT_TRUE(!region.contains({15, 8}));
  EXPECT_TRUE(region.contains({15, 7}));
  EXPECT_TRUE(!region.contains({-1, 7}));
  EXPECT_TRUE(!region.contains({1, 7}));
  EXPECT_TRUE(region.contains({5, 0}));
  EXPECT_TRUE(region.contains({5, 7}));
  EXPECT_TRUE(region.contains({4, 7}));
  EXPECT_TRUE(!region.contains({4, 8}));
  EXPECT_TRUE(region.contains({15, 0}));
}

TEST(RegionTest, ContainTest1) {
  Region region(8, {4, 4}, {16, 2});
  EXPECT_TRUE(region.contains({4, 4}));
  EXPECT_TRUE(region.contains({4, 7}));
  EXPECT_TRUE(!region.contains({4, 8}));
  EXPECT_TRUE(region.contains({11, 0}));
  EXPECT_TRUE(region.contains({11, 7}));
  EXPECT_TRUE(region.contains({16, 1}));
  EXPECT_TRUE(!region.contains({16, 2}));
}

} // namespace
