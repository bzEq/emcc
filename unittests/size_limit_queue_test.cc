#include "support/size_limit_queue.h"

#include <gtest/gtest.h>

namespace {

using namespace emcc;
TEST(SizeLimitQueueTest, Basic) {
  SizeLimitQueue<int> q(3);
  q.PushBack(0);
  q.PushBack(1);
  q.PushBack(2);
  q.PushBack(3);
  EXPECT_TRUE(q.size() == 3);
  EXPECT_TRUE(*(q.Pop()) == 1);
  EXPECT_TRUE(*(q.PopBack()) == 3);
  EXPECT_TRUE(*(q.Pop()) == 2);
}

} // namespace
