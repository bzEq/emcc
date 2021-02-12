#include "tui/wysiwyg_editor.h"

namespace emcc::tui {

bool WYSIWYGEditor::SendCommand(const Command &cmd) {
  return command_queue_->put(cmd);
}

} // namespace emcc::tui
