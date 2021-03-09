#include "editor/line_view.h"

namespace emcc::editor {

void LineView::ReCompute() {
  hlist_.clear();
  size_t point;
  parent_->ComputePoint(lineno_, 0, point);
  UTF8Decoder utf8dec;
  for (size_t rune_point = point; point < parent_->CountChars(); ++point) {
    char ch;
    parent_->Get(point, ch);
    if (utf8dec.Decode(ch)) {
      hlist_.emplace_back(CreateCharView(rune_point, utf8dec.codepoint()));
      // See https://tools.ietf.org/html/rfc3629
      // > In UTF-8, characters from the U+0000..U+10FFFF range (the UTF-16
      // > accessible range) are encoded using sequences of 1 to 4 octets.
      assert((point + 1) - rune_point <= 4);
      rune_point = point + 1;
    }
    if (ch == editor::MonoBuffer::kNewLine)
      break;
  }
  ReCompute(0);
}

void LineView::ReCompute(size_t seg) {
  seg = std::min(seg, segment_index_.size());
  size_t start_from = 0;
  if (!segment_index_.empty())
    start_from = segment_index_[seg];
  segment_index_.resize(seg);
  size_t current_offset = 0;
  for (size_t i = start_from; i < hlist_.size(); ++i) {
    auto &cv = hlist_.at(i);
    size_t hlen = cv.length();
    size_t s = current_offset + hlen;
    size_t fill_current_segment_badness =
        abs_diff(s, width_) + (s > width_ ? hlen : 0);
    size_t fill_next_segment_badness = abs_diff(hlen, width_);
    if (fill_current_segment_badness < fill_next_segment_badness) {
      current_offset = s;
    } else {
      // std::cout << "Break at " << i << std::endl;
      current_offset = hlen;
      segment_index_.emplace_back(i);
    }
  }
}

} // namespace emcc::editor
