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

class DynamicArray : public SplayRope<DynamicArrayPiece> {
public:
  using Super = SplayRope<DynamicArrayPiece>;
  using PieceTy = Super::PieceTy;

  Value &At(size_t i) {
    assert(i < Super::size());
    PieceTy &piece = Super::At(i);
    return piece.value;
  }
};

} // namespace emcc
