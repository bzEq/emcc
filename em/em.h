#pragma once

#include "editor/buffer_view.h"
#include "support/chan.h"
#include "tui/cursor.h"
#include "tui/terminal.h"

#include <atomic>

namespace emcc {

using SignalQueueTy = GoChan<int>;

class Window {
public:
  using Cursor = emcc::tui::Cursor;
  explicit Window(int height, int width, emcc::editor::BufferView &view,
                  emcc::tui::ANSITerminal &vt)
      : height_(height), width_(width), view_(view), vt_(vt), c_(0, 0),
        view_reference_row_(0), have_to_stop_(false), status_(0) {}

  int Run();
  bool MoveUp();
  bool MoveRight();
  bool MoveDown();
  bool MoveLeft();
  void ScrollUp(int num_rows);
  void ScrollDown(int num_rows);

private:
  void Show();
  bool LeftFindValidCharView(emcc::tui::Cursor &c);
  Cursor ToWindowCursor(emcc::tui::Cursor);
  Cursor ToBufferViewCursor(emcc::tui::Cursor);
  int buffer_height() const { return height_; }
  bool GetCharView(Cursor c, emcc::editor::CharView &cv);
  void Consume(const char *input, int n);

  int height_, width_;
  emcc::editor::BufferView &view_;
  emcc::tui::ANSITerminal &vt_;
  emcc::tui::Cursor c_;
  int view_reference_row_;
  std::atomic<bool> have_to_stop_;
  int status_;
};

} // namespace emcc
