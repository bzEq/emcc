#pragma once

#include <sys/ioctl.h>
#include <unistd.h>

#include "editor/buffer_view.h"
#include "tui/cursor.h"
#include "tui/terminal.h"

#include <ncurses.h>
#include <string>

namespace emcc::tui {

void RenderBufferView(emcc::editor::BufferView &view,
                      emcc::tui::ANSITerminal &vt, int height, int width);

void RenderBufferViewWithNCurses(emcc::editor::BufferView &view, WINDOW *window,
                                 int height, int width);

} // namespace emcc::tui
