// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "support/chan.h"
#include "support/misc.h"
#include "tui/basis.h"

namespace emcc::tui {

class Terminal {
public:
  Terminal(int in, int out, int w, int h, StreamTy input_receiver)
      : input_fd_(in), output_fd_(out), width_(w), height_(h),
        input_receiver_(input_receiver) {}

  bool is_open() const;

  void LoopInput();

private:
  void DisplayErrorMessage(const std::string &msg);
  bool HaveToStop();

  int input_fd_, output_fd_;
  int width_, height_;
  StreamTy input_receiver_;
};

} // namespace emcc::tui
