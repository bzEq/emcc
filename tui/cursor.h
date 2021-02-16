#pragma once

#include "support/chan.h"
#include "support/misc.h"

namespace emcc::tui {

struct Cursor {
  int y, x;
  Cursor() : y(), x() {}
  Cursor(int y, int x) : y(y), x(x) {}
  bool operator==(const Cursor &other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const Cursor &other) const { return !(*this == other); }
  static bool IsBeyond(Cursor pos, Cursor boundary) {
    if (pos.y < 0 || pos.x < 0)
      return true;
    return pos.y >= boundary.y || pos.x >= boundary.x;
  }
  static int ComputeDistance(const int wrap_width, Cursor start, Cursor end) {
    assert(wrap_width > 0);
    assert(start.x < wrap_width && end.x < wrap_width);
    return (end.y - start.y) * wrap_width + (end.x - start.x);
  }
  static Cursor Goto(const int wrap_width, Cursor origin, int distance) {
    assert(wrap_width > 0);
    int dx = distance % wrap_width;
    int dy = distance / wrap_width;
    if (origin.x + dx < 0) {
      dx += wrap_width;
      dy -= 1;
    } else if (origin.x + dx >= wrap_width) {
      dy += 1;
    }
    return Cursor{
        origin.y + dy,
        (origin.x + dx) % wrap_width,
    };
  }
};

inline Cursor operator+(const Cursor lhs, const Cursor rhs) {
  return {lhs.y + rhs.y, lhs.x + rhs.x};
}

} // namespace emcc::tui
