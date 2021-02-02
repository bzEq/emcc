// Copyright (c) 2019 Kai Luo <gluokai@gmail.com>. All rights reserved.

#include "support/misc.h"
#include "support/result.h"

#include <gtest/gtest.h>
#include <string>

namespace {

using namespace emcc;

using Status = Result<void, std::string>;

Status TestOk() { return Ok(); }

Status TestErr() { return Err("{}: It's a big mistake!", 42); }

Result<int, int> TestIntOk() { return Ok(0); }
Result<int, int> TestIntErr() { return Err(0); }

TEST(ResultTest, Functional) {
  auto v = TestOk();
  EXPECT_TRUE(v);
  auto e = TestErr();
  EXPECT_FALSE(e);
}

} // namespace
