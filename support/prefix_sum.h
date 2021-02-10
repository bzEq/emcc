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
  PrefixSumInfo() = default;
};
} // namespace

template <typename Num>
class PrefixSum : public DynamicArray<PrefixSumInfo<Num>> {
public:
  PrefixSum() = default;

  bool Add(size_t i, Num delta) {
    Node *const node = Super::AtOrNull(i);
    if (node == nullptr)
      return false;
    node->value.value += delta;
    return true;
  }

  bool Insert(size_t i, Num x) {
    ElementTy value;
    value.value = x;
    return Super::Insert(i, std::move(value));
  }

  Num At(size_t i) {
    Node *const node = Super::AtOrNull(i);
    if (node == nullptr)
      return Num();
    return node->value.value;
  }

  Num GetPrefixSum(size_t i) {
    Node *const node = Super::AtOrNull(i);
    if (!node)
      return Num();
    return GetLeftSum(node) + node->value.value;
  }

private:
  using ElementTy = PrefixSumInfo<Num>;
  using Super = DynamicArray<ElementTy>;
  using Node = typename Super::Node;
  Num GetLeftSum(Node *const node) {
    assert(node);
    if (node->left == nullptr)
      return Num();
    return node->left->value.sum;
  }

  virtual void Update(Node *const node) {
    ElementTy &info = node->value;
    info.sum = info.value + (node->left ? node->left->value.sum : Num()) +
               (node->right ? node->right->value.sum : Num());
    Super::Update(node);
  }
};

} // namespace emcc