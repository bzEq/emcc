#include "layout/paragraph.h"

#include <gtest/gtest.h>

namespace {

using namespace emcc::layout;

TEST(LayoutTest, Build) {
  HListTy hlist;
  hlist.emplace_back(1);
  hlist.emplace_back(2);
  hlist.emplace_back(4);
  hlist.emplace_back(8);
  hlist.emplace_back(16);
  ParagraphBuilder builder(17);
  auto p = builder.Build(std::move(hlist));
  EXPECT_TRUE(p->height() == 2);
  EXPECT_TRUE(p->GetLine(0).size() == 4);
  EXPECT_TRUE(p->GetLine(1).size() == 1);
}

TEST(LayoutTest, Build1) {
  HListTy hlist;
  hlist.emplace_back(1);
  hlist.emplace_back(2);
  hlist.emplace_back(4);
  hlist.emplace_back(8);
  hlist.emplace_back(16);
  ParagraphBuilder builder(15);
  auto p = builder.Build(std::move(hlist));
  EXPECT_TRUE(p == nullptr);
}

TEST(LayoutTest, Build2) {
  HListTy hlist;
  hlist.emplace_back(1);
  hlist.emplace_back(2);
  hlist.emplace_back(4);
  hlist.emplace_back(8);
  ParagraphBuilder builder(15);
  auto p = builder.Build(std::move(hlist));
  EXPECT_TRUE(p->height() == 1);
}

} // namespace
