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

struct Region {
  const size_t width;
  Cursor begin, end;
  Region(width, begin, end) : width(width), begin(begin), end(end) {
    assert(begin.x < width && begin.x >= 0);
    assert(end.x < width && end.x >= 0);
    assert((end.y == begin.y && end.x >= begin.x) || end.y > begin.y);
  }
  size_t size() const { return Cursor::ComputeDistance(width, begin, end) + 1; }
  bool contains(Cursor c) {
    if (end.y == begin.y) {
      return c.x >= begin.x && c.x <= end.x;
    }
    if (c.y == begin.y)
      return c.x >= begin.x && c.x < width;
    if (c.y == end.y)
      return c.x >= 0 && c.x <= end.x;
    return c.x >= 0 && c.x < width && c.y > begin.y && c.y < end.y;
  }
};

} // namespace emcc::tui
