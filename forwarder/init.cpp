#include <bambuzzled/log.hpp>

#if defined(_WIN32)

#include <windows.h>

BOOL WINAPI DllMain(
  HINSTANCE instance,
  DWORD reason,
  LPVOID reserved
) {
  (void)reserved;
  if (reason == DLL_PROCESS_ATTACH) {
    bbzl::log::set_level_filter(bbzl::log::log_level::TRACE);
  }
  return true;
}

#else

[[gnu::constructor]]
static void ctor() {
  bbzl::log::set_level_filter(bbzl::log::log_level::TRACE);
}

#endif
