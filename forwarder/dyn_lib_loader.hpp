#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <mutex>

#if defined(_WIN32) || defined(__CYGWIN__)
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#else
#error "Unsupported platform"
#endif

namespace bbzl::forwarder {

class dyn_lib_loader {
private:
#if defined(_WIN32) || defined(__CYGWIN__)
  HMODULE handle = nullptr;
#else
  void* handle = nullptr;
#endif

  std::string lib_path;
  std::unordered_map<std::string, void*> cached_symbols;
  std::mutex mutex;

  bool ensure_loaded() {
    if (handle != nullptr) return true;

#if defined(_WIN32) || defined(__CYGWIN__)
    handle = LoadLibraryA(lib_path.c_str());
#elif defined(__APPLE__) || defined(__linux__)
    handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    #endif

    if (handle == nullptr) {
#if defined(_WIN32) || defined(__CYGWIN__)
      DWORD error = GetLastError();
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

      std::string message(buf, size);

      throw std::runtime_error("Failed to load library: " + lib_path + ", error: " + message);
#else
      throw std::runtime_error("Failed to load library: " + lib_path + ", error: " + dlerror());
#endif
      return false;
    }

    return true;
  }

public:
  dyn_lib_loader(const std::string& lib_path) : lib_path(lib_path) {
    ensure_loaded();
  }

  ~dyn_lib_loader() {
    if (handle) {
  #if defined(_WIN32) || defined(__CYGWIN__)
      FreeLibrary(handle);
  #else
      dlclose(handle);
  #endif
    }
  }

  template<typename T>
  T get_symbol(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);

    auto it = cached_symbols.find(name);
    if (it != cached_symbols.end()) {
      return reinterpret_cast<T>(it->second);
    }

    if (!ensure_loaded()) {
      throw std::runtime_error("library not loaded: " + lib_path);
    }

    void* addr = nullptr;
  #if defined(_WIN32) || defined(__CYGWIN__)
    addr = reinterpret_cast<void*>(GetProcAddress(handle, name.c_str()));
  #else
    addr = dlsym(handle, name.c_str());
  #endif

    if (addr == nullptr) {
  #if defined(_WIN32) || defined(__CYGWIN__)
      DWORD error = GetLastError();
      throw std::runtime_error("failed to get symbol '" + name + "', error code: " + std::to_string(error));
  #else
      throw std::runtime_error("failed to get symbol '" + name + "', error: " + dlerror());
  #endif
    }

    cached_symbols[name] = addr;
    return reinterpret_cast<T>(addr);
  }
};

}
