#pragma once

#include "support/prefix_sum.h"

#include <assert.h>
#include <iostream>

namespace emcc::tui {

struct Cursor {
  int x, y;
  Cursor() : x(), y() {}
  Cursor(int x, int y) : x(x), y(y) {}
  bool operator<(const Cursor &other) const {
    if (y < other.y)
      return true;
    if (y == other.y)
      return x < other.x;
    return false;
  }
  bool operator==(const Cursor &other) const {
    return x == other.x && y == other.y;
  }
  bool operator>(const Cursor &other) const {
    if (y > other.y)
      return true;
    if (y == other.y)
      return x > other.x;
    return false;
  }
};

inline int WrapDistance(const int wrap_width, Cursor a, Cursor b) {
  assert(wrap_width > 0);
  assert(a.x < wrap_width && b.x < wrap_width);
  return (b.y - a.y) * wrap_width + b.x - a.x;
}

inline Cursor JumpTo(const int wrap_width, Cursor origin, int distance) {
  assert(wrap_width > 0);
  int dx = distance % wrap_width;
  int dy = distance / wrap_width;
  if (origin.x + dx < 0) {
    dx += wrap_width;
    dy -= 1;
  }
  return Cursor{
      origin.x + dx,
      origin.y + dy,
  };
}

// Used to map char's logical index to view cursor.
// TODO: By using binary search can we find index via cursor.
class Block {
public:
  // Get start cursor of char indexed by i.
  Cursor GetLoc(size_t i) {
    if (i == 0)
      return anchor_;
    int distance = index_.GetPrefixSum(i - 1);
    return JumpTo(width_, anchor_, distance);
  }

  bool Insert(size_t i, int width) {
    assert(width > 0);
    return index_.Insert(i, width);
  }

  size_t GetIndex(Cursor c);

private:
  const int width_;
  const Cursor anchor_;
  PrefixSum<int> index_;
};

} // namespace emcc::tui
