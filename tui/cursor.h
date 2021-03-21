#pragma once

#include <cassert>

namespace emcc::tui {

struct Cursor {
  int y, x;
  Cursor() : y(), x() {}
  Cursor(int y, int x) : y(y), x(x) {}
  bool operator==(const Cursor &other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(const Cursor &other) const { return !(*this == other); }
  // Deprecated, use Region::contains.
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

// TODO: Add iterator.
class Region {
public:
  Region(size_t width, Cursor begin, Cursor end)
      : width_(width), begin_(begin), end_(end) {
    assert(begin.x < width && begin.x >= 0);
    assert(end.x < width && end.x >= 0);
    assert((end.y == begin.y && end.x >= begin.x) || end.y > begin.y);
  }
  const Cursor &begin_cursor() const { return begin_; }
  const Cursor &end_cursor() const { return end_; }
  size_t width() const { return width_; }
  size_t size() const { return Cursor::ComputeDistance(width_, begin_, end_); }
  bool contains(Cursor c) {
    if (end_.y == begin_.y) {
      return c.x >= begin_.x && c.x < end_.x;
    }
    if (c.y == begin_.y)
      return c.x >= begin_.x && c.x < width_;
    if (c.y == end_.y)
      return c.x >= 0 && c.x < end_.x;
    return c.x >= 0 && c.x < width_ && c.y > begin_.y && c.y < end_.y;
  }

  class iterator {
  public:
    friend class Region;
    iterator &operator++() {
      c_ = Cursor::Goto(parent_.width(), c_, 1);
      return *this;
    }
    bool operator!=(const iterator &other) { return c_ != other.c_; }
    const Cursor &operator*() const { return c_; }

  private:
    iterator(const Region &parent) : parent_(parent) {}
    const Region &parent_;
    Cursor c_;
  };

  iterator begin() const {
    iterator it(*this);
    it.c_ = begin_;
    return it;
  }
  iterator end() const {
    iterator it(*this);
    it.c_ = end_;
    return it;
  }

private:
  const size_t width_;
  const Cursor begin_;
  const Cursor end_;
};

} // namespace emcc::tui
