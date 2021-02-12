// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/splay_rope.h"

#include <cassert>
#include <iostream>
#include <vector>

namespace emcc {

template <typename Num>
struct PrefixSumPiece : public DefaultPiece {
  Num value, prefix_sum, sum;
  using Super = DefaultPiece;
  PrefixSumPiece() = default;

  template <typename T>
  static void UpdateNode(SplayRopeNode<T> *node) {
    assert(node);
    Super::UpdateNode(node);
    PrefixSumPiece &piece = node->piece;
    piece.prefix_sum =
        piece.value + (node->left ? node->left->piece.sum : Num());
    piece.sum =
        piece.prefix_sum + (node->right ? node->right->piece.sum : Num());
  }
};

template <typename Num>
class PrefixSum : public SplayRope<PrefixSumPiece<Num>> {
public:
  using Super = SplayRope<PrefixSumPiece<Num>>;
  using PieceTy = typename Super::PieceTy;

  Num At(size_t i) {
    if (i >= Super::size())
      return Num();
    PieceTy &piece = Super::At(i);
    return piece.value;
  }

  PrefixSum &Insert(size_t i, Num x) {
    PieceTy piece;
    piece.value = x;
    Super::Insert(i, std::move(piece));
    return *this;
  }

  Num GetPrefixSum(size_t i) {
    if (i >= Super::size())
      return Num();
    PieceTy &piece = Super::At(i);
    return piece.prefix_sum;
  }

  bool Add(size_t i, Num delta) {
    if (i >= Super::size())
      return false;
    PieceTy &piece = Super::At(i);
    piece.value += delta;
    return true;
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
};

} // namespace emcc
