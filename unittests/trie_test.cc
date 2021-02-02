#include "tsl/htrie_map.h"

#include <functional>
#include <gtest/gtest.h>

namespace {

using DelegateType = std::function<void(void)>;

TEST(HTrieTest, Basic) {
  char c;
  tsl::htrie_map<char, DelegateType> commands = {
      {"AA", [&] { c = 'A'; }},
      {"BB", [&] { c = 'B'; }},
  };
  commands["AA"]();
  EXPECT_TRUE(c == 'A');
  commands["BB"]();
  EXPECT_TRUE(c == 'B');
}

} // namespace
