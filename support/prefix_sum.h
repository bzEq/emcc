// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/dynamic_array.h"

#include <cassert>
#include <iostream>
#include <vector>

namespace emcc {

namespace {
template <typename Num>
struct PrefixSumInfo {
  Num value, sum;
};
} // namespace

template <typename Num>
class PrefixSum : public DynamicArray<PrefixSumInfo<Num>> {
public:
  using ElementTy = PrefixSumInfo<Num>;
  using Super = DynamicArray<ElementTy>;
  using DynamicArrayElementTy = DynamicArrayElement<ElementTy>;

  bool Add(size_t i, Num delta) {
    Node *const node = Super::AtOrNull(i);
    if (node == nullptr)
      return false;
    node->value.array_element.value += delta;
    Update(node);
    return true;
  }

  PrefixSum &Insert(size_t i, Num x) {
    ElementTy value;
    value.value = x;
    Super::Insert(i, std::move(value));
    return *this;
  }

  Num At(size_t i) {
    DynamicArrayElementTy &value = Super::At(i);
    return value.array_element.value;
  }

  Num GetPrefixSum(size_t i) {
    Node *const node = Super::AtOrNull(i);
    if (!node)
      return Num();
    return GetLeftSum(node) + node->value.array_element.value;
  }

  // Find first i, GetPrefixSum(i) >= x.
  size_t LowerBound(Num x) {
    size_t l = 0, r = Super::size(), mid = l + (r - l) / 2;
    while (l < r) {
      if (GetPrefixSum(mid) >= x) {
        r = mid;
      } else {
        l = mid + 1;
      }
      mid = l + (r - l) / 2;
    }
    return r;
  }

  size_t UpperBound(Num x) {
    size_t l = 0, r = Super::size(), mid = l + (r - l) / 2;
    while (l < r) {
      if (GetPrefixSum(mid) > x) {
        r = mid;
      } else {
        l = mid + 1;
      }
      mid = l + (r - l) / 2;
    }
    return r;
  }

private:
  using Node = typename Super::Node;

  Num GetLeftSum(Node *const node) {
    assert(node);
    if (node->left == nullptr)
      return Num();
    return node->left->value.array_element.sum;
  }

  virtual void Update(Node *const node) override {
    Super::Update(node);
    ElementTy &element = node->value.array_element;
    element.sum = element.value +
                  (node->left ? node->left->value.array_element.sum : Num()) +
                  (node->right ? node->right->value.array_element.sum : Num());
  }
};

} // namespace emcc
