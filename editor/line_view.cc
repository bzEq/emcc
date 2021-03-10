#include "editor/line_view.h"

namespace emcc::editor {

void LineView::Reset(size_t lineno) {
  segment_index_.clear();
  hlist_.clear();
  size_t point;
  parent_->ComputePoint(lineno, 0, point);
  UTF8Decoder utf8dec;
  for (size_t rune_point = point; point < parent_->size(); ++point) {
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
  seg = segment_index_.empty() ? 0 : std::min(seg, segment_index_.size() - 1);
  size_t start_from = 0;
  if (!segment_index_.empty()) {
    start_from = segment_index_[seg];
  }
  segment_index_.resize(seg);
  size_t i = start_from, j = i, current_offset = 0;
  for (; j < hlist_.size(); ++j) {
    auto &cv = hlist_.at(j);
    // std::cout << cv.rune << " " << cv.length() << std::endl;
    size_t hlen = cv.length();
    size_t s = current_offset + hlen;
    size_t fill_current_segment_badness = s > width_ ? ~0 : abs_diff(s, width_);
    size_t fill_next_segment_badness = abs_diff(hlen, width_);
    if (fill_current_segment_badness <= fill_next_segment_badness) {
      current_offset = s;
    } else {
      // std::cout << "Break at: " << i << std::endl;
      current_offset = hlen;
      segment_index_.emplace_back(i);
      i = j;
    }
    // std::cout << "current offset: " << current_offset << std::endl;
    // std::cout << "current segment size: " << segment_index_.size() <<
    // std::endl;
  }
  if (i != j)
    segment_index_.emplace_back(i);
}

} // namespace emcc::editor
