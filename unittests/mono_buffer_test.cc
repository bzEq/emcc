#include "edit/mono_buffer.h"

#include <gtest/gtest.h>

namespace {
using namespace emcc;

TEST(MonoBufferTest, Append) {
  MonoBuffer mb;
  mb.Append('a');
  EXPECT_TRUE(mb.CountLines() == 1);
  mb.Append('\n');
  EXPECT_TRUE(mb.CountLines() == 1);
  mb.Append('b');
  EXPECT_TRUE(mb.CountLines() == 2);
}

} // namespace
