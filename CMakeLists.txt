cmake_minimum_required(VERSION 4.0.1)

project(
  bambuzzled
  LANGUAGES C CXX
)

option(BBZL_VENDOR_NLOHMANN_JSON "Use vendored nlohmann-json" OFF)
option(BBZL_VENDOR_OPENSSL "Use vendored OpenSSL" OFF)
option(BBZL_VENDOR_MOSQUITTO "Use vendored Mosquitto" OFF)
option(BBZL_STATIC "Statically link all dependencies" OFF)

if(BBZL_VENDOR_NLOHMANN_JSON)
  list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/nlohmann-json")
endif()

if(BBZL_VENDOR_OPENSSL)
  list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/openssl")
endif()

if(BBZL_VENDOR_MOSQUITTO)
  list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/libmosquitto")
endif()

if(BBZL_STATIC)
  set(OPENSSL_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_LIBS ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
  add_definitions("-D_CRT_SECURE_NO_DEPRECATE")
endif()

add_subdirectory(shared)
add_subdirectory(src)
add_subdirectory(video)
add_subdirectory(forwarder)
add_subdirectory(test-client)
