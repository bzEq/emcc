#include "core/mono_buffer.h"

#include <gtest/gtest.h>

namespace {
using namespace emcc;
using namespace emcc::editor;

TEST(MonoBufferTest, Append) {
  MonoBuffer mb;
  mb.Append('a');
  EXPECT_TRUE(mb.NumLines() == 1);
  mb.Append('\n');
  EXPECT_TRUE(mb.NumLines() == 1);
  mb.Append('b');
  EXPECT_TRUE(mb.NumLines() == 2);
}

TEST(MonoBufferTest, InsertTest) {
  MonoBuffer mb;
  mb.Append('a');
  mb.Append('b');
  mb.Append('\n');
  mb.Append('c');
  mb.Append('d');
  mb.Insert(0, 1, 'e');
  std::string line;
  mb.GetLine(0, 80, line);
  EXPECT_TRUE(line == "aeb\n");
  EXPECT_TRUE(mb.NumLines() == 2);
  EXPECT_TRUE(mb.size() == 6);
  line.clear();
  EXPECT_TRUE(mb.GetLine(1, 80, line) == 2);
  EXPECT_TRUE(line == "cd");
}

TEST(MonoBufferTest, EraseTest) {
  MonoBuffer mb;
  mb.Append('a');
  mb.Append('b');
  mb.Append('c');
  mb.Append('d');
  EXPECT_TRUE(mb.NumLines() == 1);
  EXPECT_TRUE(mb.Erase(0, 0, 8) == 4);
  EXPECT_TRUE(mb.NumLines() == 0);
}

TEST(MonoBufferTest, EraseTest1) {
  MonoBuffer mb;
  mb.Append('a');
  mb.Append('b');
  mb.Append('c');
  mb.Append('d');
  EXPECT_TRUE(mb.NumLines() == 1);
  EXPECT_TRUE(mb.Erase(0, 0, 1) == 1);
  EXPECT_TRUE(mb.NumLines() == 1);
}

TEST(MonoBufferTest, EraseTest2) {
  MonoBuffer mb;
  mb.Append('a');
  mb.Append('\n');
  mb.Append('b');
  mb.Append('c');
  EXPECT_TRUE(mb.Erase(0, 0, 2) == 2);
  EXPECT_TRUE(mb.NumLines() == 1);
  std::string s;
  mb.GetLine(0, 80, s);
  EXPECT_TRUE(s == "bc");
}

} // namespace
