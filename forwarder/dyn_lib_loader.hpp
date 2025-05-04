#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <format>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#else
#error "Unsupported platform"
#endif

namespace bbzl::forwarder {

class dyn_lib_loader {
private:
  #if defined(_WIN32)
  HMODULE handle = nullptr;
  #else
  void* handle = nullptr;
  #endif

  std::string lib_path;
  std::unordered_map<std::string, void*> cached_symbols;
  std::mutex mutex;

  void ensure_loaded() {
    if (handle != nullptr) {
      return;
    }

    #if defined(_WIN32)
    handle = LoadLibraryA(lib_path.c_str());
    #elif defined(__APPLE__) || defined(__linux__)
    handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    #endif

    if (handle != nullptr) {
      return;
    }

    #if defined(_WIN32)
    auto message = bbzl::utils::win32::get_err_msg();
    #else
    auto message = std::string(dlerror());
    #endif

    throw std::runtime_error(std::format(
      "Failed to load library '{}': {}",
      lib_path,
      message
    ));
  }

public:
  dyn_lib_loader(const std::string& lib_path) : lib_path(lib_path) {
    ensure_loaded();
  }

  ~dyn_lib_loader() {
    if (handle != nullptr) {
      #if defined(_WIN32)
      (void)FreeLibrary(handle);
      #else
      (void)dlclose(handle);
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

    ensure_loaded();

    void* addr = nullptr;
    #if defined(_WIN32)
    addr = static_cast<void*>(GetProcAddress(handle, name.c_str()));
    #else
    addr = dlsym(handle, name.c_str());
    #endif

    if (addr == nullptr) {
      #if defined(_WIN32)
      auto message = bbzl::utils::win32::get_err_msg();
      #else
      auto message = std::string(dlerror());
      #endif

      throw std::runtime_error(std::format(
        "failed to get symbol '{}', error code: {}",
        name,
        message
      ));
    }

    cached_symbols[name] = addr;
    return reinterpret_cast<T>(addr);
  }
};

}
