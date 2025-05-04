cmake_minimum_required(VERSION 4.0.1)

project(
  bambuzzled
  LANGUAGES CXX
)

include(${PROJECT_SOURCE_DIR}/cmake/bbzl-private-functions.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
  add_definitions("-D_CRT_SECURE_NO_DEPRECATE")
  add_compile_options("/Zc:preprocessor")
endif()

set(BBZL_WITH_NLOHMANN_JSON "SYSTEM" CACHE STRING "Build with nlohmann-json")
set_property(CACHE BBZL_WITH_NLOHMANN_JSON PROPERTY STRINGS "SYSTEM" "VENDOR")
string(TOUPPER "${BBZL_WITH_NLOHMANN_JSON}" BBZL_WITH_NLOHMANN_JSON)

set(BBZL_WITH_MOSQUITTO "SYSTEM" CACHE STRING "Build with mosquitto")
set_property(CACHE BBZL_WITH_MOSQUITTO PROPERTY STRINGS "SYSTEM" "VENDOR")
string(TOUPPER "${BBZL_WITH_MOSQUITTO}" BBZL_WITH_MOSQUITTO)

set(BBZL_WITH_CURL "SYSTEM" CACHE STRING "Build with curl")
set_property(CACHE BBZL_WITH_CURL PROPERTY STRINGS "SYSTEM" "VENDOR")
string(TOUPPER "${BBZL_WITH_CURL}" BBZL_WITH_CURL)

set(BBZL_STATIC_DEPS "AUTO" CACHE STRING "Statically link all dependencies")
set_property(CACHE BBZL_STATIC_DEPS PROPERTY STRINGS "AUTO" ON OFF)
string(TOUPPER "${BBZL_STATIC_DEPS}" BBZL_STATIC_DEPS)

if(BBZL_WITH_NLOHMANN_JSON STREQUAL "SYSTEM")
elseif(BBZL_WITH_NLOHMANN_JSON STREQUAL "VENDOR")
  list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/nlohmann_json")
else()
  message(FATAL_ERROR "BBZL_WITH_NLOHMANN_JSON must be either SYSTEM or VENDOR")
endif()

if(BBZL_WITH_MOSQUITTO STREQUAL "SYSTEM")
  list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/mosquitto/system")
elseif(BBZL_WITH_MOSQUITTO STREQUAL "VENDOR")
  list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/mosquitto/vendor")
else()
  message(FATAL_ERROR "BBZL_WITH_MOSQUITTO must be either SYSTEM or VENDOR")
endif()

if(BBZL_WITH_CURL STREQUAL "SYSTEM")
elseif(BBZL_WITH_CURL STREQUAL "VENDOR")
  list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/curl")
else()
  message(FATAL_ERROR "BBZL_WITH_CURL must be either SYSTEM or VENDOR")
endif()

function(warn_system_dep OPT_NAME DEP_NAME)
  if(${${OPT_NAME}} STREQUAL "SYSTEM")
    message(
      AUTHOR_WARNING
      "Static dependencies requested but ${OPT_NAME} is set to SYSTEM."
      " ${DEP_NAME} may not be linked statically."
    )
  endif()
endfunction()

if(
  BBZL_STATIC_DEPS STREQUAL "ON"
  OR BBZL_STATIC_DEPS STREQUAL "TRUE"
  OR BBZL_STATIC_DEPS STREQUAL "1"
)
  bbzl_set_if_undefined(Boost_USE_STATIC_LIBS ON)
  bbzl_set_if_undefined(MOSQUITTO_USE_STATIC_LIBS ON)
  bbzl_set_if_undefined(OPENSSL_USE_STATIC_LIBS ON)
  bbzl_set_if_undefined(CURL_USE_STATIC_LIBS ON)
elseif(
  BBZL_STATIC_DEPS STREQUAL "OFF"
  OR BBZL_STATIC_DEPS STREQUAL "FALSE"
  OR BBZL_STATIC_DEPS STREQUAL "0"
)
  bbzl_set_if_undefined(Boost_USE_STATIC_LIBS OFF)
  bbzl_set_if_undefined(MOSQUITTO_USE_STATIC_LIBS OFF)
  bbzl_set_if_undefined(OPENSSL_USE_STATIC_LIBS OFF)
  bbzl_set_if_undefined(CURL_USE_STATIC_LIBS OFF)
elseif(BBZL_STATIC_DEPS STREQUAL "AUTO")
else()
  message(FATAL_ERROR "Invalid value for BBZL_STATIC_DEPS")
endif()

if(MOSQUITTO_USE_STATIC_LIBS)
  warn_system_dep(BBZL_WITH_MOSQUITTO libmosquitto)
endif()

if(CURL_USE_STATIC_LIBS)
  warn_system_dep(BBZL_WITH_CURL libcurl)
endif()

add_subdirectory(shared)
add_subdirectory(network-plugin)
add_subdirectory(video-plugin)
add_subdirectory(forwarder)
