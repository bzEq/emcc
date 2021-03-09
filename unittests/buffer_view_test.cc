#include "editor/buffer_view.h"

#include <gtest/gtest.h>
#include <iostream>

namespace {

using namespace emcc::editor;

TEST(BufferViewTest, Basic) {
  std::string msg = "你好世界\n"
                    "こんにちは世界\n"
                    "Hello, world\n";
  MonoBuffer buffer;
  buffer.Append(msg.data(), msg.length());
  BufferView view(&buffer, 16, 80);
  EXPECT_TRUE(view.NumRows() == 3);
  EXPECT_TRUE(view.NumLines() == 3);
}

TEST(BufferViewTest, Basic1) {
  std::string msg = "你好世界\n"
                    "Hello, world\n"
                    "こんにちは世界\n";
  MonoBuffer buffer;
  buffer.Append(msg.data(), msg.length());
  BufferView view(&buffer, 3, 8);
  EXPECT_TRUE(view.NumLines() == 2);
  EXPECT_TRUE(view.NumRows() == 4);
}

TEST(BufferViewTest, Basic2) {
  std::string msg = "你好世界\n"
                    "こんにちは世界\n"
                    "Hello, world\n";
  MonoBuffer buffer;
  buffer.Append(msg.data(), msg.length());
  BufferView view(&buffer, 3, 4);
  EXPECT_TRUE(view.NumRows() == 3);
  EXPECT_TRUE(view.NumLines() == 1);
}

} // namespace
