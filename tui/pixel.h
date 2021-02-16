#pragma once

#include <vector>

namespace emcc::tui {

// Graphical representation of a charater, also offers map to character's
// logical position.
struct Pixel {
  struct {
    int character;
  } shade;

  struct {
    uint8_t head_tail_pair; // A character might span multiple pixels.
    size_t point; // Point is the offset of the character at text buffer.
  } position;

  Pixel() { Reset(); }

  void Reset() {
    shade.character = ' ';
    memset(&position, -1, sizeof(position));
  }

  bool operator==(const Pixel &other) {
    return memcmp(this, &other, sizeof(*this)) == 0;
  }

  Pixel &operator=(const Pixel &other) {
    memcpy(this, &other, sizeof(*this));
    return *this;
  }

  bool is_same_position(const Pixel &other) const {
    return memcmp(&this->position, &other.position, sizeof(this->position)) ==
           0;
  }

  bool is_same_shade(const Pixel &other) const {
    return memcmp(&this->shade, &other.shade, sizeof(this->shade)) == 0;
  }

  bool is_head() const {
    return position.head_tail_pair >> (sizeof(position.head_tail_pair) * 8 - 1);
  }
  size_t offset() const {
    if (is_head())
      return 0;
    return position.head_tail_pair &
           (static_cast<decltype(position.head_tail_pair)>(~0U) >> 1);
  }
  size_t length() const {
    assert(is_head());
    return offset();
  }
  void set_offset(size_t len, size_t offset) {
    if (offset == 0)
      position.head_tail_pair =
          (1 << (sizeof(position.head_tail_pair) * 8 - 1)) | len;
    else
      position.head_tail_pair = offset;
  }
  static Pixel MakeHeadPixel(size_t point, size_t len, int c = 0) {
    Pixel p;
    assert(len < (1 << (sizeof(p.position.head_tail_pair) * 8 - 1)));
    p.position.head_tail_pair =
        (1 << (sizeof(p.position.head_tail_pair) * 8 - 1)) | len;
    p.position.point = point;
    p.shade.character = c;
    return p;
  }
  static Pixel MakeTailPixel(size_t point, size_t offset, int c = 0) {
    Pixel p;
    assert(offset > 0 &&
           offset < (1 << (sizeof(p.position.head_tail_pair) * 8 - 1)));
    p.position.head_tail_pair = offset;
    p.position.point = point;
    p.shade.character = c;
    return p;
  }
  static std::vector<Pixel> MakeSeries(size_t offset, size_t len, int c = 0) {
    assert(len > 0);
    std::vector<Pixel> ret;
    ret.emplace_back(MakeHeadPixel(offset, len, c));
    for (size_t i = 1; i < len; ++i)
      ret.emplace_back(MakeTailPixel(offset, i, c));
    return ret;
  }
};

} // namespace emcc::tui
