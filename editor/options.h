// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#pragma once

#include <string>
#include <vector>

namespace emcc {

struct Options {
  std::string extension_load_path;
  std::vector<std::string> files_to_open;
};

} // namespace emcc
