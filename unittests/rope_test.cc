// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "support/random.h"
#include "support/rope.h"
#include "support/sys.h"

#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <time.h>

namespace {

using Rope = emcc::Rope<char, 2>;

std::string GenerateRandomString(size_t size) {
  static size_t offset = 0;
  std::string result;
  result.reserve(size);
  for (size_t i = 0; i < size; ++i) {
    result.push_back('a' + (offset++) % 26);
  }
  return result;
}

TEST(RopeTest, CompareString) {
  std::string s(GenerateRandomString(1 << 20));
  Rope rope;
  rope.Append(s);
  EXPECT_TRUE(rope.size() == s.size());
  EXPECT_TRUE(rope == s);
}

void RandomInsertTest() {
  emcc::Random rnd(std::time(nullptr));
  Rope rope;
  std::string original_string;
  for (int i = 0; i < (1 << 10); ++i) {
    std::string buffer =
        GenerateRandomString(static_cast<size_t>(1024 * rnd.Next()));
    size_t index = rope.size() * rnd.Next();
    original_string.insert(original_string.begin() + index, buffer.begin(),
                           buffer.end());
    rope.Insert(index, buffer);
  }
  ASSERT_TRUE(rope == original_string);
}

void RandomInsertOneCharTest() {
  emcc::Random rnd(std::time(nullptr));
  Rope rope;
  std::string original_string;
  for (int i = 0; i < (1 << 10); ++i) {
    std::string buffer =
        GenerateRandomString(static_cast<size_t>(1 * rnd.Next()));
    size_t index = rope.size() * rnd.Next();
    original_string.insert(original_string.begin() + index, buffer.begin(),
                           buffer.end());
    rope.Insert(index, buffer);
  }
  ASSERT_TRUE(rope == original_string);
}

TEST(RopeTest, RandomInsertion) {
  for (int i = 0; i < (1 << 4); ++i) {
    RandomInsertTest();
  }
}

TEST(RopeTest, RandomOneCharInsertion) {
  for (int i = 0; i < (1 << 10); ++i) {
    RandomInsertOneCharTest();
  }
}

TEST(RopeTest, EraseTest) {
  Rope rope;
  rope.Append("what the hell it is?!");
  EXPECT_TRUE(rope.Erase(1, 4) == 4);
  EXPECT_TRUE(rope.Erase(2, 3) == 3);
  EXPECT_TRUE(rope.Erase(3, 3) == 3);
  EXPECT_TRUE(rope == ("wth it is?!"));
}

TEST(RopeTest, EraseTest1) {
  emcc::Random rnd(std::time(nullptr));
  Rope rope(GenerateRandomString(1 << 24));
  while (rope.size()) {
    size_t len = (1 << 16) * rnd.Next();
    size_t index = rope.size() * rnd.Next();
    size_t to_erase = std::min(len, rope.size() - index);
    EXPECT_TRUE(rope.Erase(index, len) == to_erase);
  }
}

TEST(RopeTest, EraseTest2) {
  Rope rope, erased;
  rope.Append("what the hell it is?!");
  EXPECT_TRUE(rope.Erase(0, 4, &erased) == 4);
  EXPECT_TRUE(erased == ("what"));
}

TEST(RopeTest, ClearTest) {
  Rope rope;
  rope.Append("wtf");
  EXPECT_TRUE(rope == "wtf");
  rope.clear();
  EXPECT_TRUE(rope == "");
}

TEST(RopeTest, RealEdit) {
  Rope rope;
  rope.Insert(16, "Hllo");
  rope.Insert(1, "e");
  EXPECT_TRUE(rope == "Hello");
  rope.Insert(rope.size(), ", world!");
  EXPECT_TRUE(rope == "Hello, world!");
  rope.Insert(6, " ");
  EXPECT_TRUE(rope == "Hello,  world!");
  rope.Insert(1024, "!!");
  EXPECT_TRUE(rope == "Hello,  world!!!");
  rope.Insert(0, "!!");
  EXPECT_TRUE(rope == "!!Hello,  world!!!");
  rope.Insert(1, "lol");
  EXPECT_TRUE(rope == "!lol!Hello,  world!!!");
}

TEST(RopeTest, ConcatTest) {
  Rope lhs, rhs;
  lhs.Append("Hey, ");
  rhs.Append("Jude!");
  lhs.Concat(std::move(rhs));
  EXPECT_TRUE(lhs == "Hey, Jude!");
  EXPECT_TRUE(rhs == "");
}

TEST(RopeTest, SplitTest) {
  Rope lhs;
  lhs.Append("Hey, Jude!");
  Rope rhs(lhs.Split(5));
  EXPECT_TRUE(lhs == "Hey, ");
  EXPECT_TRUE(rhs == "Jude!");
}

} // namespace
