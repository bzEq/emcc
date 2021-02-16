// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "edit/mono_buffer.h"
#include "support/chan.h"
#include "support/misc.h"
#include "tui/cursor.h"
#include "tui/input.h"
#include "tui/page.h"
#include "tui/renderer.h"

namespace emcc::tui {

class StreamInterpreter;
class Terminal;

class WYSIWYGEditor {
public:
  WYSIWYGEditor(Page *page, MonoBuffer *buffer, NcursesRenderer *renderer)
      : changed_(false), loc_{0, 0}, page_(page), buffer_(buffer),
        input_(nullptr), renderer_(renderer), have_to_stop_(false), status_(0) {
  }

  WYSIWYGEditor(Page *page, MonoBuffer *buffer, NcursesInput *input,
                NcursesRenderer *renderer)
      : changed_(false), loc_{0, 0}, page_(page), buffer_(buffer),
        input_(input), renderer_(renderer), have_to_stop_(false), status_(0) {}
  Cursor loc() const { return loc_; }
  void MoveTo(Cursor loc) {
    if (!Cursor::IsBeyond(loc, page_->GetBoundary()))
      loc_ = loc;
  }
  int Run();
  void Insert(char c);
  void DeleteForward();
  void Backspace();
  void KillLine();
  void KillRegion();
  void Yank();
  void GotoLine(size_t line);
  void ForwardChar();
  void ForwardWord();
  void BackwardChar();
  void BackwardWord();
  void NextLine();
  void PreviousLine();
  void MoveBeginningOfLine();
  void MoveEndOfLine();
  void MoveUp();
  void MoveDown();
  void MoveLeft();
  void MoveRight();
  void Show();

private:
  void Handle(int);
  bool changed_;
  Cursor loc_;
  Page *page_;
  MonoBuffer *buffer_;
  NcursesInput *input_;
  NcursesRenderer *renderer_;
  bool have_to_stop_;
  int status_;
};

} // namespace emcc::tui
