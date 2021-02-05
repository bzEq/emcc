#include "editor/line_buffer.h"

#include <gtest/gtest.h>

namespace {
using namespace emcc;

TEST(LineBufferTest, AppendTest) {
  LineBuffer lb;
  lb.Append('a');
  EXPECT_TRUE(lb.CountLines() == 1);
  lb.Append('\n');
  EXPECT_TRUE(lb.CountLines() == 1);
  EXPECT_TRUE(lb.Verify());
}

TEST(LineBufferTest, GetLineTest) {
  LineBuffer lb;
  lb.Append('a');
  lb.Append('\n');
  std::string line;
  lb.GetLine(0, 80, line);
  EXPECT_TRUE(line == "a\n");
  EXPECT_TRUE(lb.Verify());
}

TEST(LineBufferTest, InsertTest) {
  LineBuffer lb;
  lb.Append('a');
  lb.Append('b');
  lb.Append('\n');
  lb.Append('c');
  lb.Append('d');
  lb.Insert(0, 1, 'e');
  std::string line;
  lb.GetLine(0, 80, line);
  EXPECT_TRUE(line == "aeb\n");
  EXPECT_TRUE(lb.Verify());
}

TEST(LineBufferTest, InsertTest1) {
  LineBuffer lb;
  lb.Insert(2, ~0, 'a');
  std::string line;
  lb.GetLine(2, ~0, line);
  EXPECT_TRUE(line == "a");
  EXPECT_TRUE(lb.CountLines() == 3);
  line.clear();
  lb.GetLine(0, ~0, line);
  EXPECT_TRUE(line == "\n");
  line.clear();
  lb.GetLine(1, ~0, line);
  EXPECT_TRUE(line == "\n");
  EXPECT_TRUE(lb.Verify());
}

TEST(LineBufferTest, EraseTest) {
  LineBuffer lb;
  lb.Append('a');
  lb.Append('b');
  lb.Append('c');
  lb.Append('d');
  EXPECT_TRUE(lb.CountLines() == 1);
  EXPECT_TRUE(lb.Erase(0, 0, 8) == 4);
  EXPECT_TRUE(lb.CountLines() == 0);
  EXPECT_TRUE(lb.Verify());
}

TEST(LineBufferTest, EraseTest1) {
  LineBuffer lb;
  lb.Append('a');
  lb.Append('b');
  lb.Append('c');
  lb.Append('d');
  EXPECT_TRUE(lb.CountLines() == 1);
  EXPECT_TRUE(lb.Erase(0, 0, 1) == 1);
  EXPECT_TRUE(lb.CountLines() == 1);
  EXPECT_TRUE(lb.Verify());
}

TEST(LineBufferTest, EraseTest2) {
  LineBuffer lb;
  lb.Append('a');
  lb.Append('\n');
  lb.Append('b');
  lb.Append('c');
  EXPECT_TRUE(lb.Erase(0, 0, 2) == 2);
  EXPECT_TRUE(lb.CountLines() == 1);
  std::string s;
  lb.GetLine(0, 80, s);
  EXPECT_TRUE(s == "bc");
  EXPECT_TRUE(lb.Verify());
}

TEST(LineBufferTest, FuzzFailure) {
  LineBuffer lb;
  for (size_t i = 0; i < 4096; ++i) {
    lb.Insert(0, 0, '0');
  }
  lb.Insert(0, ~0, LineBuffer::kNewLine);
  for (size_t i = 0; i < 4096; ++i) {
    lb.Insert(1, 0, '0');
  }
  lb.Insert(1, ~0, LineBuffer::kNewLine);
  lb.Erase(0, 4096, 4096);
  EXPECT_TRUE(lb.Verify());  
  lb.Erase(0, 4096, 4096);
  EXPECT_TRUE(lb.Verify());
}

} // namespace
