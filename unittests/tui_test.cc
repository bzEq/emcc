#include "editor/mono_buffer.h"
#include "support/misc.h"
#include "tui/buffer_view.h"
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

TEST(BufferViewTest, ResizeTest) {
  emcc::editor::MonoBuffer buffer;
  const char text[] = "hello\n"
                      "rofl\n"
                      "lmao\n";
  buffer.Append(text, emcc::GetArrayLength(text));
  BufferView view(&buffer);
  view.Resize(2, 4);
  EXPECT_TRUE(view.GetPixel({0, 3}).shade.character == (int)'l');
  EXPECT_TRUE(view.GetPixel({1, 0}).shade.character == (int)'o');
  EXPECT_TRUE(view.GetPixel({1, 1}).shade.character == (int)'\n');
  EXPECT_TRUE(view.height() == 2);
}

TEST(BufferView, EmptyFramebuffer) {
  emcc::editor::MonoBuffer buffer;
  const char text[] = "hello\n"
                      "rofl\n"
                      "lmao\n";
  buffer.Append(text, emcc::GetArrayLength(text));
  BufferView view(&buffer);
  view.RewriteFrameBuffer(0, ~0);
  EXPECT_TRUE(view.height() == 0);
}

TEST(BufferView, MoveTest) {
  emcc::editor::MonoBuffer buffer;
  const char text[] = "hello\n"
                      "rofl\n"
                      "lmao\n";
  buffer.Append(text, emcc::GetArrayLength(text));
  BufferView view(&buffer);
  view.Resize(1, 4);
  EXPECT_TRUE(view.GetPixel({0, 0}).shade.character == (int)'h');
  view.MoveDown();
  EXPECT_TRUE(view.cursor().y == 0);
  EXPECT_TRUE(view.cursor().x == 0);
  EXPECT_TRUE(view.GetPixel({0, 0}).shade.character == (int)'o');
  view.MoveRight();
  view.MoveUp();
  EXPECT_TRUE(view.GetPixel(view.cursor()).shade.character == (int)'e');
}

TEST(BufferView, MoveTest1) {
  emcc::editor::MonoBuffer buffer;
  const char text[] = "hello\n"
                      "rofl\n"
                      "lmao\n";
  buffer.Append(text, emcc::GetArrayLength(text));
  BufferView view(&buffer);
  view.Resize(2, 4);
  EXPECT_TRUE(view.GetPixel(view.cursor()).shade.character == (int)'h');
  view.MoveDown();
  EXPECT_TRUE(view.cursor().y == 1);
  EXPECT_TRUE(view.cursor().x == 0);
  EXPECT_TRUE(view.GetPixel({1, 0}).shade.character == (int)'o');
  view.MoveDown();
  EXPECT_TRUE(view.GetPixel(view.cursor()).shade.character == (int)'r');
  view.MoveUp();
  view.MoveUp();
  EXPECT_TRUE(view.GetPixel(view.cursor()).shade.character == (int)'h');
}

} // namespace
