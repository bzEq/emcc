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
    uint8_t is_head : 1; // A character might span multiple pixels.
    uint8_t offset : 7;
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

  bool is_head() const { return position.is_head; }
  size_t offset() const {
    if (is_head())
      return 0;
    return position.offset;
  }
  size_t length() const {
    assert(is_head());
    return position.offset;
  }
  void set_offset(size_t len, size_t offset) {
    if (offset == 0) {
      position.is_head = 1;
      position.offset = len;
    } else {
      position.is_head = 0;
      position.offset = offset;
    }
  }
  static Pixel MakeHeadPixel(size_t point, size_t len, int c = 0) {
    Pixel p;
    p.position.is_head = 1;
    p.position.offset = len;
    p.position.point = point;
    p.shade.character = c;
    return p;
  }
  static Pixel MakeTailPixel(size_t point, size_t offset, int c = 0) {
    Pixel p;
    p.position.is_head = 0;
    p.position.offset = offset;
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
