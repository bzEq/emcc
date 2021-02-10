// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "editor/line_buffer.h"
#include "support/chan.h"
#include "support/misc.h"
#include "tui/basis.h"

namespace emcc::tui {

class StreamInterpreter;
class Terminal;

class App {
public:
  App(Terminal &input, StreamInterpreter &interpreter, Terminal &output,
      LineBuffer &buffer)
      : input_(input), interpreter_(interpreter), output_(output),
        buffer_(buffer) {}

  bool SendCommand(const Command &cmd);

private:
  void LoopCommand();

  Terminal &input_;
  StreamInterpreter &interpreter_;
  CommandQueueTy command_queue_;
  Terminal &output_;
  LineBuffer &buffer_;
};

} // namespace emcc::tui
