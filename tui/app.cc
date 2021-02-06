#include "tui/app.h"

namespace emcc::tui {

bool App::SendCommand(const Command &cmd) { return command_queue_->put(cmd); }

} // namespace emcc::tui
