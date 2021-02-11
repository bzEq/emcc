#pragma once

#include "support/splay_rope.h"

namespace emcc {
template <typename Value>
struct DynamicArrayPiece : public DefaultPiece {
  Value value;
  using Super = DefaultPiece;
  template <typename... Args>
  DynamicArrayPiece(Args &&...args) : value(std::forward<Args>(args)...) {}
};

template <typename Value>
class DynamicArray : public SplayRope<DynamicArrayPiece<Value>> {
public:
  using Super = SplayRope<DynamicArrayPiece<Value>>;
  using PieceTy = typename Super::PieceTy;

  Value &At(size_t i) {
    assert(i < Super::size());
    PieceTy &piece = Super::At(i);
    return piece.value;
  }
};

} // namespace emcc
