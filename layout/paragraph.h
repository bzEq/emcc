#pragma once

#include "layout/hbox.h"

#include <assert.h>
#include <memory>
#include <vector>

namespace emcc::layout {

using HListTy = std::vector<HBox>;
using VListTy = std::vector<HListTy>;

class Paragraph {
public:
  friend class ParagraphBuilder;

  size_t height() const { return vlist_.size(); }

  const HListTy &GetLine(size_t i) const {
    assert(i < height());
    return vlist_[i];
  }

private:
  VListTy vlist_;
};

class ParagraphBuilder {
public:
  explicit ParagraphBuilder(size_t width) : width_(width) {}
  std::unique_ptr<Paragraph> Build(HListTy &&hlist);

private:
  size_t width_;
};

} // namespace emcc::layout
