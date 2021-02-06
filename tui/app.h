// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/chan.h"
#include "support/misc.h"
#include "tui/basis.h"

namespace emcc::tui {

class StreamInterpreter;
class Terminal;

class App {
  Terminal &input_;
  StreamInterpreter &interpreter_;
  Shared<Chan<Command, 8>> command_queue_;
  Terminal &output_;

public:
  App(Terminal &input, StreamInterpreter &interpreter, Terminal &output)
      : input_(input), interpreter_(interpreter), output_(output) {}

  bool SendCommand(const Command &cmd);
};

} // namespace emcc::tui
