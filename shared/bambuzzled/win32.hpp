#pragma once

#if defined(_WIN32)

#include <string>
#include <windows.h>

namespace bbzl::utils::win32 {

[[nodiscard]]
std::string get_err_msg(DWORD error = GetLastError()) noexcept;

[[nodiscard]]
std::string getenv_utf8(char const* name) noexcept;

}

#endif
