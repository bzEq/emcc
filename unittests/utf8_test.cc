#include "support/utf8.h"
#include "support/wcwidth.h"

#include <gtest/gtest.h>
#include <iostream>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <vector>

namespace {

using namespace emcc;

TEST(UTF8Test, Basic) {
  std::string s("abcdefg");
  size_t l = 0;
  uint32_t state = 0, codepoint;
  for (auto c : s) {
    if (DecodeUTF8(&state, &codepoint, c))
      ++l;
  }
  EXPECT_TRUE(l == s.size());
}

TEST(UTF8Test, CN) {
  std::string s("你好世界");
  size_t l = 0;
  uint32_t state = 0, codepoint;
  for (auto c : s) {
    if (DecodeUTF8(&state, &codepoint, c))
      ++l;
  }
  EXPECT_TRUE(state == UTF8_ACCEPT);
  EXPECT_TRUE(l == 4);
}

TEST(UTF8Test, WCWIDTH) {
  std::string s("你好世界");
  std::vector<uint32_t> us;
  EXPECT_TRUE(DecodeUTF8(s, us));
  EXPECT_TRUE(us.size() == 4);
  EXPECT_TRUE(wchar_width(us[0]) == 2);
  EXPECT_TRUE(wchar_width(us[1]) == 2);
  EXPECT_TRUE(wchar_width(us[2]) == 2);
  EXPECT_TRUE(wchar_width(us[3]) == 2);
}

} // namespace
