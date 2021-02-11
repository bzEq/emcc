// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/splay_rope.h"

namespace emcc {

template <typename Element>
struct DynamicArrayElement {
  size_t current_index;
  Element array_element;
  template <typename... Args>
  DynamicArrayElement(Args &&...args)
      : current_index(~0), array_element(std::forward<Args>(args)...) {}
};

template <typename Element>
class DynamicArray : public SplayRope<DynamicArrayElement<Element>> {
public:
  using Super = SplayRope<DynamicArrayElement<Element>>;
  using ValueTy = DynamicArrayElement<Element>;

protected:
  using Node = typename Super::Node;
  virtual void Update(Node *const node) override {
    Super::Update(node);
    ValueTy &e = node->value;
    e.current_index = node->GetLeftSize();
  }
};
} // namespace emcc
