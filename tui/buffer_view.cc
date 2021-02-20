#include "tui/buffer_view.h"
#include "support/chan.h"
#include "support/defer.h"

namespace emcc::tui {

void BufferView::Resize(size_t height, size_t width) {
  if (!height || !width)
    return;
  // TODO: Recover cursor_ after resizing.
  if (width != width_) {
    framebuffer_ = Framebuffer(width);
    width_ = width;
  }
  framebuffer_.Resize(height);
  framebuffer_.Reset(framebuffer_.region());
  size_t point;
  buffer_->ComputePoint(baseline_, 0, point);
  RewriteFrameBuffer(point, ~0, framebuffer_, framebuffer_.region());
}

void BufferView::RewriteFrameBuffer(size_t point, size_t len, Framebuffer &fb,
                                    Region region) {
  assert(width() == fb.width());
  if (region.width() != fb.width())
    return;
  size_t endpoint = std::min(buffer_->CountChars(), point + len);
  Cursor at = region.begin_cursor();
  for (; point != endpoint && region.contains(at); ++point) {
    char c;
    buffer_->Get(point, c);
    int rep;
    size_t length;
    std::tie(rep, length) = GetCharAndWidth(c);
    assert(length <= width());
    // Do not break character's pixels into multiple lines.
    if (at.x + length > width()) {
      fb.Reset({width_, {at.y, at.x}, {at.y + 1, 0}});
      ++at.y;
      at.x = 0;
      if (!region.contains(at))
        break;
    }
    // Check if fb is capable to fill pixel at `at`.
    if (fb.height() <= at.y)
      fb.Resize(at.y + 1);
    for (size_t j = 0; j < length; ++j) {
      assert(at.y < fb.height());
      assert(at.x < width());
      Pixel &pixel = fb.GetPixel(at);
      pixel.shade.character = rep;
      pixel.position.point = point;
      pixel.set_offset(length, j);
    }
    if (c == MonoBuffer::kNewLine) {
      if (at.x + 1 < width_)
        fb.Reset({width_, {at.y, at.x + (int)length}, {at.y + 1, 0}});
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

bool BufferView::GetStatusLine(std::string &content) const {
  if (framebuffer_.empty())
    return false;
  const Pixel &at = GetPixel(cursor_);
  size_t line, col;
  buffer_->ComputePosition(at.position.point, line, col);
  content =
      fmt::format("| {} | p: {} of {} | l: {} of {} | c: {} of {} |",
                  buffer_->filename(), at.position.point, buffer_->CountChars(),
                  line, buffer_->CountLines(), col, buffer_->GetLineSize(line));
  return true;
}

bool BufferView::MoveLeft() {
  Cursor probe = cursor_;
  probe.x -= 1;
  if (probe.x < 0)
    return false;
  Pixel px;
  if (!framebuffer_.GetPixel(probe, px) ||
      px.position.point == MonoBuffer::npos)
    return false;
  if (!px.is_head())
    probe.x -= px.offset();
  assert(probe.x >= 0);
  cursor_ = probe;
  return true;
}

bool BufferView::MoveRight() {
  Cursor probe = cursor_;
  Pixel px;
  if (!framebuffer_.GetPixel(probe, px) ||
      px.position.point == MonoBuffer::npos) {
    return false;
  }
  assert(px.is_head());
  probe.x += px.length();
  if (!framebuffer_.GetPixel(probe, px) ||
      px.position.point == MonoBuffer::npos) {
    return false;
  }
  cursor_ = probe;
  return true;
}

bool BufferView::MoveUp() {
  Cursor probe = cursor_;
  if (probe.y == 0) {
    if (!ScrollUp()) {
      return false;
    }
  } else {
    probe.y -= 1;
  }
  Pixel px;
  for (; probe.x >= 0; --probe.x) {
    if (!framebuffer_.GetPixel(probe, px))
      return false;
    if (px.position.point != MonoBuffer::npos) {
      cursor_ = probe;
      return true;
    }
  }
  return false;
}

bool BufferView::MoveDown() {
  Cursor probe = cursor_;
  if (probe.y + 1 >= framebuffer_.height()) {
    if (!ScrollDown()) {
      return false;
    }
  } else {
    ++probe.y;
  }
  Pixel px;
  for (; probe.x >= 0; --probe.x) {
    if (!framebuffer_.GetPixel(probe, px))
      return false;
    if (px.position.point != MonoBuffer::npos) {
      cursor_ = probe;
      return true;
    }
  }
  return false;
}

bool BufferView::ScrollDown() {
  if (framebuffer_.empty())
    return false;
  Pixel px;
  if (!framebuffer_.GetPixel({(int)framebuffer_.height() - 1, 0}, px))
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
  FramebufferTy next_fb(width_);
  RewriteFrameBuffer(
      next_framebuffer_start_point, buffer_->GetLineSize(line, line + 1),
      next_fb,
      Region(width_, {0, 0},
             Cursor(std::numeric_limits<decltype(cursor_.y)>::max(), 0)));
  if (next_fb.empty()) {
    return false;
  }
  Pixel replica;
  int i = 0;
  for (; i < next_fb.height(); ++i) {
    if (next_fb.GetPixel({i, 0}, replica) && replica.is_same_position(px))
      break;
  }
  if (i >= next_fb.height() - 1)
    return false;
  if (framebuffer_.empty())
    return false;
  for (size_t j = 0; j < framebuffer_.height() - 1; ++j) {
    framebuffer_.SwapLine(j, j + 1);
  }
  Framebuffer::SwapLine(framebuffer_, framebuffer_.height() - 1, next_fb,
                        i + 1);
  return true;
}

// TODO: Optimize scrolling in ScrollUp(size_t).
// FIXME: Would be f**ked if previous logical line is very long.
bool BufferView::ScrollUp() {
  Pixel px;
  if (!framebuffer_.GetPixel({0, 0}, px)) {
    return false;
  }
  size_t point = px.position.point;
  if (point == MonoBuffer::npos) {
    return false;
  }
  size_t line, col;
  buffer_->ComputePosition(point, line, col);
  if (line != 0) {
    line = line - 1;
  }
  col = 0;
  size_t prev_framebuffer_start_point;
  buffer_->ComputePoint(line, col, prev_framebuffer_start_point);
  FramebufferTy prev_fb(width_);
  RewriteFrameBuffer(
      prev_framebuffer_start_point, point - prev_framebuffer_start_point,
      prev_fb,
      Region(width_, {0, 0},
             Cursor(std::numeric_limits<decltype(cursor_.y)>::max(), 0)));
  if (prev_fb.empty()) {
    return false;
  }
  if (framebuffer_.empty())
    return false;
  for (size_t j = framebuffer_.height() - 1; j != 0; --j) {
    framebuffer_.SwapLine(j, j - 1);
  }
  Framebuffer::SwapLine(framebuffer_, 0, prev_fb, prev_fb.height() - 1);
  return true;
}

} // namespace emcc::tui
