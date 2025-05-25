#include "win32.hpp"

#include <string>
#include <cstddef>
#include <windows.h>

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

  auto s = std::string(buf, size);
  (void)LocalFree(buf);
  return s;
}

std::string getenv_utf8(char const* name) noexcept {
  int wide_name_len = MultiByteToWideChar(
    CP_UTF8,
    0,
    name,
    -1,
    nullptr,
    0
  );
  if (wide_name_len <= 0) {
    return "";
  }

  auto wide_name = std::wstring(wide_name_len, 0);
  (void)MultiByteToWideChar(
    CP_UTF8,
    0,
    name,
    -1,
    wide_name.data(),
    wide_name_len
  );

  wchar_t const* wide_value = _wgetenv(wide_name.c_str());
  if (wide_value == nullptr) {
    return "";
  }

  int value_len = WideCharToMultiByte(
    CP_UTF8,
    0,
    wide_value,
    -1,
    nullptr,
    0,
    nullptr,
    nullptr
  );
  if (value_len <= 0) {
    return "";
  }

  auto result = std::string(value_len, 0);
  (void)WideCharToMultiByte(
    CP_UTF8,
    0,
    wide_value,
    -1,
    result.data(),
    value_len,
    nullptr,
    nullptr
  );

  if (!result.empty() && result.back() == '\0') {
    result.pop_back();
  }

  return result;
}

}
