#pragma once

#include "editor/buffer_view.h"
#include "tui/cursor.h"

namespace emcc {

class Window {
public:
  using Cursor = emcc::tui::Cursor;
  explicit Window(int height, int width, emcc::editor::BufferView &view)
      : height_(height), width_(width), view_(view), c_(0, 0),
        view_reference_row_(0) {}

  void Refresh();
  void LoopInput();
  bool MoveUp();
  bool MoveRight();
  bool MoveDown();
  bool MoveLeft();
  void ScrollUp(int num_rows);
  void ScrollDown(int num_rows);

private:
  bool LeftFindValidCharView(emcc::tui::Cursor &c);
  Cursor ToWindowCursor(emcc::tui::Cursor);
  Cursor ToBufferViewCursor(emcc::tui::Cursor);
  int buffer_height() const { return height_; }

  int height_, width_;
  emcc::editor::BufferView &view_;
  emcc::tui::Cursor c_;
  int view_reference_row_;
};

} // namespace emcc
