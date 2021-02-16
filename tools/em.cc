// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "edit/mono_buffer.h"
#include "support/chan.h"
#include "support/misc.h"
#include "support/sys.h"
#include "tui/wysiwyg_editor.h"

#include <csignal>

using namespace emcc;
using namespace emcc::tui;

static Arc<SignalQueueTy> signal_queue = std::make_shared<SignalQueueTy>();

static void SendSignal(int signum) { signal_queue->put_nowait(signum); }

int main(int argc, char *argv[]) {
  if (argc != 2) {
    Die("Usage: {} <filename>");
  }
  std::string filename(argv[1]);
  auto buffer = MonoBuffer::CreateFromFile(filename);
  if (!buffer)
    Die("Failed to open {}", filename);
  std::signal(SIGWINCH, SendSignal);
  initscr();
  atexit([]() { endwin(); });
  raw();
  noecho();
  keypad(stdscr, true);
  NcursesRenderer renderer(stdscr);
  NcursesInput input(stdscr);
  int height, width;
  renderer.GetMaxYX(height, width);
  Framebuffer framebuffer(width, height);
  Page page(buffer.get(), &framebuffer, width, height);
  page.set_baseline(0);
  WYSIWYGEditor editor(signal_queue, &page, buffer.get(), &input, &renderer);
  return editor.Run();
}
