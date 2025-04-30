#include <memory>

#include "demangle.hpp"

namespace bbzl::utils {

#if defined (_WIN32) && defined(_MSC_VER)

#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

std::string demangle(const char* name) {
  if (name[0] != '?') {
    return name;
  }

  char undecorated_name[1024];
  if (UnDecorateSymbolName(
    name,
    undecorated_name,
    sizeof(undecorated_name),
    UNDNAME_COMPLETE
  )) {
    return undecorated_name;
  }

  return name;
}

#else

#include <cxxabi.h>

std::string demangle(const char* name) {
  int status = 0;
  std::unique_ptr<char, void(*)(void*)> res {
    abi::__cxa_demangle(name, nullptr, nullptr, &status),
    std::free
  };
  return status == 0 ? res.get() : name;
}

#endif

}
