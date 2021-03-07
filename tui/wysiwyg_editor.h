// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "editor/mono_buffer.h"
#include "support/chan.h"
#include "support/misc.h"
#include "tui/cursor.h"
#include "tui/input.h"
#include "tui/renderer.h"

namespace emcc::tui {

using SignalQueueTy = GoChan<int>;

class WYSIWYGEditor {
public:
  WYSIWYGEditor(BufferView *page, NcursesRenderer *renderer)
      : signal_queue_(nullptr), page_(page), input_(nullptr),
        renderer_(renderer), have_to_stop_(false), status_(0) {}

  WYSIWYGEditor(BufferView *page, NcursesInput *input,
                NcursesRenderer *renderer)
      : signal_queue_(nullptr), page_(page), input_(input), renderer_(renderer),
        have_to_stop_(false), status_(0) {}

  WYSIWYGEditor(Arc<SignalQueueTy> signal_queue, BufferView *page,
                NcursesInput *input, NcursesRenderer *renderer)
      : signal_queue_(signal_queue), page_(page), input_(input),
        renderer_(renderer), have_to_stop_(false), status_(0) {}

  int Run();
  void Show();

private:
  void Consume(int);
  void Resize();
  Arc<SignalQueueTy> signal_queue_;
  BufferView *page_;
  NcursesInput *input_;
  NcursesRenderer *renderer_;
  bool have_to_stop_;
  int status_;
};

} // namespace emcc::tui
