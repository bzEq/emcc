#include "support/splay_rope.h"

#include <gtest/gtest.h>

namespace {
using namespace emcc;

struct Element {
  size_t current_index;
};

class DynamicArray : public SplayRope<Element> {
public:
  using Super = SplayRope<Element>;

protected:
  virtual void Update(Node *const node) {
    Super::Update(node);
    Element &e = node->value;
    e.current_index = node->GetLeftSize();
  }
};

TEST(SplayRopeTest, AsDynamicArray) { DynamicArray array; }

} // namespace
