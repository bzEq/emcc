#pragma once

#include <string>

namespace emcc {

int wstring_width_cjk(const std::wstring &text);
int wstring_width(const std::wstring &text);
int wchar_width_cjk(wchar_t ucs);
int wchar_width(wchar_t ucs);

} // namespace emcc
