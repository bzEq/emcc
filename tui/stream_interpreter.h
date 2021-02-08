#pragma once

#include "tui/basis.h"

namespace emcc::tui {

class StreamInterpreter {
public:
  explicit StreamInterpreter(StreamTy input) : input_(input) {}

  void Init();

  void LoopInput();

private:
  StreamTy input_;
};

} // namespace emcc::tui
