#pragma once

#include "tui/cursor.h"
#include "tui/pixel.h"

namespace emcc::tui {

class Framebuffer {
public:
  friend class BufferView;
  explicit Framebuffer(size_t width) : width_(width) { assert(width_); }

  size_t width() const { return width_; }

  void Append() { storage_.emplace_back(width_); }

  void Resize(size_t height) {
    if (height > storage_.size())
      for (size_t i = storage_.size(); i < height; ++i)
        Append();
    else
      storage_.resize(height);
  }

  bool empty() const { return storage_.empty(); }

  size_t Reset(Region region) {
    size_t count = 0;
    if (region.width() != width())
      return count;
    auto &begin = region.begin_cursor();
    auto &end = region.end_cursor();
    if (begin.y >= height())
      return count;
    auto &start_line = storage_[begin.y];
    if (begin.y == end.y) {
      std::fill(start_line.begin() + begin.x, start_line.end() + end.x,
                Pixel());
      count += end.x - begin.x;
      return count;
    } else {
      std::fill(start_line.begin() + begin.x, start_line.end(), Pixel());
      count += width() - begin.x;
    }
    for (int i = begin.y + 1; i < end.y && i < height(); ++i) {
      auto &line = storage_[i];
      std::fill(line.begin(), line.end(), Pixel());
      count += line.size();
    }
    if (end.y >= height())
      return count;
    auto &end_line = storage_[end.y];
    std::fill(end_line.begin(), end_line.begin() + end.x, Pixel());
    count += end.x;
    return count;
  }

  size_t height() const { return storage_.size(); }

  Region region() const { return {width(), {0, 0}, {(int)height(), 0}}; }

  const Pixel &GetPixel(Cursor c) const {
    assert(c.x >= 0 && c.x < width());
    assert(c.y >= 0 && c.y < height());
    return storage_[c.y][c.x];
  }

  bool GetPixel(Cursor c, Pixel &px) {
    if (c.x < 0 || c.y < 0 || c.x >= width() || c.y >= height())
      return false;
    px = storage_[c.y][c.x];
    return true;
  }

  template <typename... Args>
  void SetPixel(Cursor c, Args &&...args) {
    assert(c.x >= 0 && c.x < width());
    assert(c.y >= 0 && c.y < height());
    auto &line = storage_[c.y];
    line.emplace(line.begin() + c.x, std::forward<Args>(args)...);
  }

  bool FindPoint(size_t point, Cursor &c) {
    for (size_t y = 0; y < storage_.size(); ++y) {
      for (size_t x = 0; x < storage_[y].size(); ++x) {
        if (storage_[y][x].position.point == point) {
          c = {(int)y, (int)x};
          return true;
        }
      }
    }
    return false;
  }

private:
  using StorageTy = std::vector<std::vector<Pixel>>;

  void SwapLine(size_t y, size_t v) {
    assert(y < height());
    assert(v < height());
    std::swap(storage_[y], storage_[v]);
  }

  static void SwapLine(Framebuffer &a, size_t ay, Framebuffer &b, size_t by) {
    if (a.width() != b.width())
      return;
    assert(ay < a.height());
    assert(by < b.height());
    std::swap(a.storage_[ay], b.storage_[by]);
  }

  Pixel &GetPixel(Cursor c) {
    assert(c.x >= 0 && c.x < width());
    assert(c.y >= 0 && c.y < height());
    return storage_[c.y][c.x];
  }

  size_t width_;
  StorageTy storage_;
};

} // namespace emcc::tui
