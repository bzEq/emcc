#include "support/defer.h"

#include <gtest/gtest.h>

namespace {

TEST(DeferTest, Smoke) {
  defer { std::cout << "Message 0\n"; };
  defer { std::cout << "Message 1\n"; };
  defer { std::cout << "Message 2\n"; };
}

} // namespace
