#pragma once

#include "support/chan.h"
#include "support/misc.h"

namespace emcc::tui {

struct Command {};

using StreamTy = Shared<Chan<char, 4096>>;

} // namespace emcc::tui
