#include "layout/paragraph.h"

#include <gtest/gtest.h>

namespace {

using namespace emcc::layout;

struct DumbBox {
  size_t len;
  size_t length() const { return len; }
};

class DumbHList : public HListTy<DumbBox> {
public:
  using Super = HListTy<DumbBox>;
  void Append(size_t i) { Super::Append(DumbBox{i}); }
};

class DumbParagraph : public Paragraph<DumbBox> {
public:
  using Super = Paragraph<DumbBox>;
  DumbParagraph(size_t width) : Super(width) {}
  DumbParagraph(size_t width, DumbHList &&hlist)
      : Super(width, std::move(hlist)) {}
  void Append(size_t i) { Super::Append(DumbBox{i}); }

  void Insert(int y, int x, size_t i) { Super::Insert(y, x, DumbBox{i}); }
};

TEST(LayoutTest, Build) {
  DumbHList hlist;
  hlist.Append(1);
  hlist.Append(2);
  hlist.Append(4);
  hlist.Append(8);
  hlist.Append(16);
  EXPECT_TRUE(hlist.size() == 5);
  auto p = std::make_unique<DumbParagraph>(17, std::move(hlist));
  EXPECT_TRUE(p->height() == 2);
  EXPECT_TRUE(p->NumBoxes(0) == 4);
  EXPECT_TRUE(p->NumBoxes(1) == 1);
}

TEST(LayoutTest, Build1) {
  DumbHList hlist;
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
  DumbHList hlist;
  hlist.Append(1);
  hlist.Append(2);
  hlist.Append(4);
  hlist.Append(8);
  EXPECT_TRUE(hlist.size() == 4);
  auto p = std::make_unique<Paragraph<DumbBox>>(15, std::move(hlist));
  EXPECT_TRUE(p->height() == 1);
  EXPECT_TRUE(p->NumBoxes(0) == 4);
}

TEST(LayoutTest, Append) {
  auto p = std::make_unique<DumbParagraph>(15);
  p->Append(1);
  p->Append(2);
  p->Append(4);
  p->Append(8);
  p->Append(15);
  EXPECT_TRUE(p->height() == 2);
  EXPECT_TRUE(p->NumBoxes(0) == 4);
  EXPECT_TRUE(p->NumBoxes(1) == 1);
  size_t c = 0;
  for (auto hbox : p->GetLine(0)) {
    EXPECT_TRUE(hbox.length() == (1 << (c++)));
  }
  for (auto hbox : p->GetLine(1)) {
    EXPECT_TRUE(hbox.length() == 15);
  }
}

TEST(LayoutTest, Insert) {
  auto p = std::make_unique<DumbParagraph>(15);
  p->Append(1);
  p->Append(4);
  p->Append(15);
  p->Insert(0, 1, 2);
  p->Insert(0, 3, 8);
  EXPECT_TRUE(p->height() == 2);
  EXPECT_TRUE(p->NumBoxes(0) == 4);
  EXPECT_TRUE(p->NumBoxes(1) == 1);
  size_t c = 0;
  for (auto hbox : p->GetLine(0)) {
    EXPECT_TRUE(hbox.length() == (1 << (c++)));
  }
  for (auto hbox : p->GetLine(1)) {
    EXPECT_TRUE(hbox.length() == 15);
  }
}

} // namespace
