#pragma once

#if defined(_WIN32)

#include <string>
#include <windows.h>

namespace bbzl::utils::win32 {

std::string get_err_msg(DWORD error = GetLastError()) noexcept;

}

#endif
