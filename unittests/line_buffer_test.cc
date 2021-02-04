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
  EXPECT_TRUE(lb.Verify());
}

// TEST(LineBufferTest, EraseTest) {
//   LineBuffer lb, erased;
//   lb.Append('a');
//   lb.Append('b');
//   lb.Append('c');
//   lb.Append('d');
//   EXPECT_TRUE(lb.CountLines() == 1);
//   lb.Erase(0, 0, 8, erased);
//   EXPECT_TRUE(lb.CountLines() == 0);
// }

// TEST(LineBufferTest, EraseTest1) {
//   LineBuffer lb, erased;
//   lb.Append('a');
//   lb.Append('b');
//   lb.Append('c');
//   lb.Append('d');
//   EXPECT_TRUE(lb.CountLines() == 1);
//   lb.Erase(0, 0, 1, erased);
//   EXPECT_TRUE(lb.CountLines() == 1);
// }

// TEST(LineBufferTest, EraseTest2) {
//   LineBuffer lb, erased;
//   lb.Append('a');
//   lb.Append('\n');
//   lb.Append('b');
//   lb.Append('c');
//   lb.Erase(0, 0, 2, erased);
//   EXPECT_TRUE(lb.CountLines() == 1);
//   std::string s;
//   lb.GetLine(0, 80, s);
//   EXPECT_TRUE(s == "bc");
// }

} // namespace
