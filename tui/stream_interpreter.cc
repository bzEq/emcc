#include "tui/stream_interpreter.h"

namespace emcc::tui {

void StreamInterpreter::LoopInput() {
  std::string sequence;
  while (true) {
    char c;
    bool ok = input_->get(c);
    if (!ok)
      break;
    sequence.push_back(c);
  }
}

} // namespace emcc::tui
