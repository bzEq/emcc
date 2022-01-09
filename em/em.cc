// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "em.h"
#include "core/mono_buffer.h"
#include "support/chan.h"
#include "support/misc.h"
#include "support/sys.h"

#include <csignal>

using namespace emcc;
using namespace emcc::tui;

static Arc<SignalQueueTy> signal_queue =
    std::make_shared<SignalQueueTy>(16, false);

static void SendSignal(int signum) { signal_queue->put(signum); }

int main(int argc, char *argv[]) {
  if (argc != 2) {
    Die("Usage: {} <filename>", argv[0]);
  }
  std::string filename(argv[1]);
  auto buffer = editor::MonoBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename);
  if (!buffer->IsUTF8Encoded())
    Die("{} is not utf8 encoded", filename);
  std::signal(SIGWINCH, SendSignal);
  return 0;
}
