// Copyright (c) 2021 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include "edit/mono_buffer.h"
#include "support/chan.h"
#include "support/misc.h"
#include "tui/basis.h"
#include "tui/page.h"
#include "tui/renderer.h"

namespace emcc::tui {

class StreamInterpreter;
class Terminal;

class WYSIWYGEditor {
public:
  WYSIWYGEditor(Page *page, MonoBuffer *buffer, NcursesRenderer *renderer)
      : changed_(false), loc_{0, 0}, page_(page), buffer_(buffer),
        renderer_(renderer) {}
  Cursor loc() const { return loc_; }
  void MoveTo(Cursor loc) {
    if (!Cursor::IsBeyond(loc, page_->GetBoundary()))
      loc_ = loc;
  }
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
  void Show();

private:
  bool changed_;
  Cursor loc_;
  Page *page_;
  MonoBuffer *buffer_;
  NcursesRenderer *renderer_;
};

} // namespace emcc::tui
