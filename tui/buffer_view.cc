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
  return GetPixel(framebuffer_, c.y, c.x, pixel);
}

bool BufferView::GetPixel(const FramebufferTy &fb, Cursor at, Pixel &px) {
  return GetPixel(fb, at.y, at.x, px);
}

bool BufferView::GetPixel(size_t y, size_t x, Pixel &px) {
  return GetPixel(framebuffer_, y, x, px);
}

bool BufferView::GetPixel(const FramebufferTy &fb, size_t y, size_t x,
                          Pixel &pixel) {
  if (y >= fb.size() || x >= width())
    return false;
  pixel = fb[y][x];
  return true;
}

void BufferView::FillFramebuffer() {
  size_t start_point;
  buffer_->ComputePoint(baseline_, 0, start_point);
  RewriteFrameBuffer(start_point, ~0, Cursor(height(), width()));
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
  ResetFrameBuffer(fb, at, {at.y, boundary.x});
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
      ResetFrameBuffer(fb, at, {at.y, boundary.x});
    }
    for (size_t j = 0; j < length; ++j) {
      assert(at.y < fb.size());
      assert(at.x < width());
      Pixel &pixel = fb[at.y][at.x];
      pixel.shade.character = rep;
      pixel.position.point = point;
      pixel.set_offset(length, j);
    }
    if (c == MonoBuffer::kNewLine) {
      ++at.y;
      at.x = 0;
      ResetFrameBuffer(fb, at, {at.y, boundary.x});
    } else {
      at = Cursor::Goto(width(), at, length);
    }
  }
}

// TODO: Handle '\t' and etc.
std::tuple<int, size_t> BufferView::GetCharAndWidth(char c) {
  return {(int)c, 1};
}

// void BufferView::UpdateStatusLine() {
//   if (framebuffer_.empty())
//     return;
//   const Pixel &at = GetPixel(cursor_.y, cursor_.x);
//   size_t line, col;
//   buffer_->ComputePosition(at.position.point, line, col);
//   std::vector<Pixel> &status_line = framebuffer_.back();
//   std::string content = fmt::format(
//       "----| {} | p: {} of {} | l: {} of {} | c: {} of {} |",
//       buffer_->filename(), at.position.point + 1, buffer_->CountChars(),
//       line + 1, buffer_->CountLines(), col + 1, buffer_->GetLineSize(line));
//   for (size_t i = 0; i < status_line.size(); ++i) {
//     if (i < content.size()) {
//       status_line[i].shade.character = content[i];
//     } else {
//       status_line[i].shade.character = '-';
//     }
//   }
// }

void BufferView::UpdateStatusLine() {}

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
  if (!GetPixel(probe, px) || px.position.point == MonoBuffer::npos) {
    return;
  }
  assert(px.is_head());
  probe.x += px.length();
  if (!GetPixel(probe, px) || px.position.point == MonoBuffer::npos) {
    return;
  }
  cursor_ = probe;
}

void BufferView::MoveUp() {
  defer { UpdateStatusLine(); };
  Cursor probe = cursor_;
  if (probe.y == 0) {
    if (!ScrollUp())
      return;
  } else {
    probe.y -= 1;
  }
  Pixel px;
  for (; probe.x >= 0; --probe.x) {
    if (!GetPixel(probe, px))
      return;
    if (px.position.point != MonoBuffer::npos) {
      cursor_ = probe;
      return;
    }
  }
}

void BufferView::MoveDown() {
  defer { UpdateStatusLine(); };
  Cursor probe = cursor_;
  if (probe.y + 1 >= framebuffer_.size()) {
    if (!ScrollDown()) {
      return;
    }
  } else {
    ++probe.y;
  }
  Pixel px;
  for (; probe.x >= 0; --probe.x) {
    if (!GetPixel(probe, px))
      return;
    if (px.position.point != MonoBuffer::npos) {
      cursor_ = probe;
      return;
    }
  }
}

bool BufferView::ScrollDown() {
  if (framebuffer_.empty())
    return false;
  Pixel px;
  if (!GetPixel(framebuffer_, {(int)framebuffer_.size() - 1, 0}, px))
    return false;
  size_t point = px.position.point;
  if (point == MonoBuffer::npos)
    return false;
  size_t line, col;
  buffer_->ComputePosition(point, line, col);
  if (line + 1 >= buffer_->CountLines())
    return false;
  size_t next_framebuffer_start_point;
  buffer_->ComputePoint(line, 0, next_framebuffer_start_point);
  FramebufferTy next_fb;
  RewriteFrameBuffer(
      next_fb, next_framebuffer_start_point,
      buffer_->GetLineSize(line, line + 1), {0, 0},
      Cursor(std::numeric_limits<decltype(cursor_.y)>::max(), width()));
  if (next_fb.empty()) {
    return false;
  }
  Pixel replica;
  int i = 0;
  for (; i < next_fb.size(); ++i) {
    if (GetPixel(next_fb, i, 0, replica) && replica.is_same_position(px))
      break;
  }
  if (i >= next_fb.size() - 1)
    return false;
  if (framebuffer_.empty())
    return false;
  for (size_t j = 0; j < framebuffer_.size() - 1; ++j) {
    framebuffer_[j] = std::move(framebuffer_[j + 1]);
  }
  framebuffer_[framebuffer_.size() - 1] = std::move(next_fb[i + 1]);
  return true;
}

// TODO: Optimize scrolling in ScrollUp(size_t).
bool BufferView::ScrollUp() {
  Pixel px;
  if (!GetPixel(framebuffer_, {0, 0}, px))
    return false;
  size_t point = px.position.point;
  if (point == MonoBuffer::npos)
    return false;
  size_t line, col;
  buffer_->ComputePosition(point, line, col);
  if (line != 0) {
    line = line - 1;
  }
  col = 0;
  size_t prev_framebuffer_start_point;
  buffer_->ComputePoint(line, col, prev_framebuffer_start_point);
  FramebufferTy prev_fb;
  RewriteFrameBuffer(
      prev_fb, prev_framebuffer_start_point,
      point - prev_framebuffer_start_point, {0, 0},
      Cursor(std::numeric_limits<decltype(cursor_.y)>::max(), width()));
  if (prev_fb.empty())
    return false;
  Pixel replica;
  int i = (int)prev_fb.size() - 1;
  for (; i >= 0; --i) {
    if (GetPixel(prev_fb, i, 0, replica) && replica.is_same_position(px))
      break;
  }
  if (i <= 0)
    return false;
  if (framebuffer_.empty())
    return false;
  for (size_t j = framebuffer_.size() - 1; j != 0; --j) {
    framebuffer_[j] = std::move(framebuffer_[j - 1]);
  }
  framebuffer_[0] = std::move(prev_fb[i - 1]);
  return true;
}

} // namespace emcc::tui
