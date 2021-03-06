#include "layout/paragraph.h"

#include <cassert>

namespace emcc::layout {
std::unique_ptr<Paragraph> ParagraphBuilder::Build(HListTy &&hlist) {
  auto result = std::make_unique<Paragraph>();
  VListTy &vlist = result->vlist_;
  HListTy current;
  size_t current_offset = 0;
  for (size_t i = 0; i < hlist.size();) {
    auto &hbox = hlist[i];
    if (hbox.len > width_)
      return nullptr;
    if (current_offset + hbox.len > width_) {
      assert(!current.empty());
      vlist.emplace_back(std::move(current));
      assert(current.empty());
      current_offset = 0;
    } else {
      current_offset += hbox.len;
      current.emplace_back(std::move(hbox));
      ++i;
    }
  }
  if (!current.empty())
    vlist.emplace_back(std::move(current));
  return result;
}
} // namespace emcc::layout
