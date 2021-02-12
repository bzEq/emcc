#pragma once

#include "support/chan.h"
#include "support/misc.h"

namespace emcc::tui {

struct Cursor {
  int y, x;
  Cursor() : y(), x() {}
  Cursor(int y, int x) : y(y), x(x) {}
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
  bool operator!=(const Cursor &other) const { return !(*this == other); }
  bool operator>(const Cursor &other) const {
    if (y > other.y)
      return true;
    if (y == other.y)
      return x > other.x;
    return false;
  }
  bool operator>=(const Cursor &other) const {
    return *this == other || *this > other;
  }
  bool operator<=(const Cursor &other) const {
    return *this == other || *this < other;
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
      origin.y + dy,
      origin.x + dx,
  };
}

struct Command {};

using StreamTy = Shared<Chan<char, 4096>>;

using CommandQueueTy = Shared<Chan<Command, 8>>;

} // namespace emcc::tui
