#include "layout/paragraph.h"

#include <gtest/gtest.h>

namespace {

using namespace emcc::layout;

struct DumbBox {
  size_t len;
  template <typename... Args>
  DumbBox(Args &&...args) : len(std::forward<Args>(args)...) {}
  DumbBox(size_t len) : len(len) {}
  DumbBox(DumbBox &&) = default;
  size_t length() const { return len; }
};

TEST(LayoutTest, Build) {
  HListTy<DumbBox> hlist;
  hlist.Append(1);
  hlist.Append(2);
  hlist.Append(4);
  hlist.Append(8);
  hlist.Append(16);
  EXPECT_TRUE(hlist.size() == 5);
  auto p = std::make_unique<Paragraph<DumbBox>>(17, std::move(hlist));
  EXPECT_TRUE(p->height() == 2);
  EXPECT_TRUE(p->NumBoxes(0) == 4);
  EXPECT_TRUE(p->NumBoxes(1) == 1);
}

TEST(LayoutTest, Build1) {
  HListTy<DumbBox> hlist;
  hlist.Append(1);
  hlist.Append(2);
  hlist.Append(4);
  hlist.Append(8);
  hlist.Append(16);
  EXPECT_TRUE(hlist.size() == 5);
  auto p = std::make_unique<Paragraph<DumbBox>>(15, std::move(hlist));
  EXPECT_TRUE(p->height() == 2);
  EXPECT_TRUE(p->NumBoxes(0) == 4);
  EXPECT_TRUE(p->NumBoxes(1) == 1);
}

TEST(LayoutTest, Build2) {
  HListTy<DumbBox> hlist;
  hlist.Append(1);
  hlist.Append(2);
  hlist.Append(4);
  hlist.Append(8);
  EXPECT_TRUE(hlist.size() == 4);
  auto p = std::make_unique<Paragraph<DumbBox>>(15, std::move(hlist));
  EXPECT_TRUE(p->height() == 1);
  EXPECT_TRUE(p->NumBoxes(0) == 4);
}

} // namespace
