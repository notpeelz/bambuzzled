#include "win32.hpp"

#if defined(_WIN32)

namespace bbzl::utils::win32 {

std::string get_err_msg(DWORD error) noexcept {
  LPSTR buf = nullptr;
  size_t size = FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    nullptr,
    error,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPSTR)&buf,
    0,
    nullptr
  );

  return std::string(buf, size);
}

}

#endif
