#include "tui/buffer_view.h"
#include "support/chan.h"
#include "support/defer.h"

namespace emcc::tui {

bool BufferView::FindPoint(size_t point, Cursor &c) {
  for (size_t y = 0; y < framebuffer_.size(); ++y) {
    for (size_t x = 0; x < framebuffer_[y].size(); ++x) {
      if (framebuffer_[y][x].position.point == point) {
        c.y = y;
        c.x = x;
        return true;
      }
    }
  }
  return false;
}

bool BufferView::GetPixel(Cursor c, Pixel &pixel) {
  return GetPixel(c.y, c.x, pixel);
}

bool BufferView::GetPixel(size_t y, size_t x, Pixel &pixel) {
  if (y >= framebuffer_.size() || x >= width())
    return false;
  pixel = framebuffer_[y][x];
  return true;
}

void BufferView::FillFramebuffer(size_t num_buffer_line) {
  size_t start_point;
  cursor_ = {0, 0};
  buffer_->ComputePoint(baseline_, 0, start_point);
  RewriteFrameBuffer(start_point, ~0, Cursor(num_buffer_line, width()));
}

void BufferView::ResetFrameBuffer(Cursor begin, Cursor end) {
  ResetFrameBuffer(framebuffer_, begin, end);
}

void BufferView::ResetFrameBuffer(FramebufferTy &fb, Cursor begin, Cursor end) {
  assert(end.y >= begin.y);
  if (end.y >= fb.size()) {
    fb.resize(end.y + 1);
  }
  auto &start_line = fb[begin.y];
  start_line.resize(width());
  if (begin.y == end.y) {
    if (begin.x < end.x) {
      std::fill(start_line.begin() + begin.x, start_line.begin() + end.x,
                Pixel());
    }
    return;
  }
  std::fill(start_line.begin() + begin.x, start_line.end(), Pixel());
  for (size_t i = begin.y + 1; i < end.y; ++i) {
    fb[i].resize(width());
    std::fill(fb[i].begin(), fb[i].end(), Pixel());
  }
  auto &end_line = fb[end.y];
  end_line.resize(width());
  std::fill(end_line.begin(), end_line.begin() + end.x, Pixel());
}

void BufferView::RewriteFrameBuffer(size_t start_point, size_t len,
                                    Cursor boundary) {
  RewriteFrameBuffer(framebuffer_, start_point, len, cursor_, boundary);
}

void BufferView::RewriteFrameBuffer(FramebufferTy &fb, size_t start_point,
                                    size_t len, Cursor at, Cursor boundary) {
  ResetFrameBuffer(fb, at, {boundary.y - 1, boundary.x});
  for (size_t i = 0; !Cursor::IsBeyond(at, boundary) && i < len; ++i) {
    // We should avoid a character span across a framebuffer line.
    size_t point = start_point + i;
    char c = '0';
    if (!buffer_->Get(point, c))
      break;
    int rep;
    size_t length;
    std::tie(rep, length) = GetCharAndWidth(c);
    assert(length <= width());
    if (at.x + length > width()) {
      ++at.y;
      at.x = 0;
    }
    for (size_t j = 0; j < length; ++j) {
      if (at.y >= fb.size())
        break;
      assert(at.x < width());
      Pixel &pixel = fb[at.y][at.x];
      pixel.shade.character = rep;
      pixel.position.point = point;
      pixel.set_offset(length, j);
    }
    if (c == MonoBuffer::kNewLine) {
      ++at.y;
      at.x = 0;
    } else {
      at = Cursor::Goto(width(), at, length);
    }
  }
}

// TODO: Handle '\t' and etc.
std::tuple<int, size_t> BufferView::GetCharAndWidth(char c) {
  return {(int)c, 1};
}

void BufferView::UpdateStatusLine() {
  if (framebuffer_.empty())
    return;
  const Pixel &at = GetPixel(cursor_.y, cursor_.x);
  size_t line, col;
  buffer_->ComputePosition(at.position.point, line, col);
  std::vector<Pixel> &status_line = framebuffer_.back();
  std::string content = fmt::format(
      "----| {} | p: {} of {} | l: {} of {} | c: {} of {} |",
      buffer_->filename(), at.position.point + 1, buffer_->CountChars(),
      line + 1, buffer_->CountLines(), col + 1, buffer_->GetLineSize(line));
  for (size_t i = 0; i < status_line.size(); ++i) {
    if (i < content.size()) {
      status_line[i].shade.character = content[i];
    } else {
      status_line[i].shade.character = '-';
    }
  }
}

void BufferView::MoveLeft() {
  defer { UpdateStatusLine(); };
  Cursor probe = cursor_;
  probe.x -= 1;
  if (probe.x < 0)
    return;
  Pixel px;
  if (!GetPixel(probe, px) || px.position.point == MonoBuffer::npos)
    return;
  if (!px.is_head())
    probe.x -= px.offset();
  assert(probe.x >= 0);
  cursor_ = probe;
}

void BufferView::MoveRight() {
  defer { UpdateStatusLine(); };
  Cursor probe = cursor_;
  Pixel px;
  if (!GetPixel(probe, px) || px.position.point == MonoBuffer::npos)
    return;
  assert(px.is_head());
  probe.x += px.length();
  if (!GetPixel(probe, px) || px.position.point == MonoBuffer::npos)
    return;
  cursor_ = probe;
}

void BufferView::MoveUp() {
  defer { UpdateStatusLine(); };
}

void BufferView::MoveDown() {
  defer { UpdateStatusLine(); };
}

void BufferView::ScrollUp(size_t num_buffer_line) {}

} // namespace emcc::tui
