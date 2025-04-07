#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <dlfcn.h>
#else
#error "Unsupported platform"
#endif

class dyn_lib_loader {
private:
#ifdef _WIN32
  HMODULE handle = nullptr;
#else
  void* handle = nullptr;
#endif

  std::string lib_path;
  std::unordered_map<std::string, void*> cached_symbols;
  std::mutex mutex;

  bool ensure_loaded() {
    if (handle != nullptr) return true;

#ifdef _WIN32
    handle = LoadLibraryA(m_libPath.c_str());
#elif defined(__APPLE__) || defined(__linux__)
    handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    #endif

    if (!handle) {
#ifdef _WIN32
      DWORD error = GetLastError();
      throw std::runtime_error("Failed to load library: " + lib_path + ", error: " + error);
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
  #ifdef _WIN32
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
  #ifdef _WIN32
    addr = reinterpret_cast<void*>(GetProcAddress(handle, name.c_str()));
  #else
    addr = dlsym(handle, name.c_str());
  #endif

    if (!addr) {
  #ifdef _WIN32
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
