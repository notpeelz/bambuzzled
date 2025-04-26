include(FindPackageHandleStandardArgs)
include(ExternalProject)
include(CMakePrintHelpers)

set(LIBMOSQUITTO_INSTALL_DIR ${CMAKE_BINARY_DIR}/vendor/libmosquitto)
set(LIBMOSQUITTO_INCLUDE_DIR ${LIBMOSQUITTO_INSTALL_DIR}/include)

file(GLOB PATCH_FILES "${CMAKE_CURRENT_SOURCE_DIR}/patches/*.patch")
set(PATCH_COMMANDS "")
foreach(PATCH_FILE ${PATCH_FILES})
  list(
    APPEND
    PATCH_COMMANDS
    patch -Np1 -i ${PATCH_FILE}
  )
endforeach()

find_package(OpenSSL REQUIRED)

set(LIBMOSQUITTO_CMAKE_ARGS "")
list(
  APPEND
  LIBMOSQUITTO_CMAKE_ARGS
  -DWITH_STATIC_LIBRARIES=ON
  -DWITH_PIC=ON
  -DWITH_THREADING=OFF
  -DWITH_CJSON=OFF
  -DWITH_CLIENTS=OFF
  -DWITH_BROKER=OFF
  -DWITH_APPS=OFF
  -DWITH_PLUGINS=OFF
  -DDOCUMENTATION=OFF
  -DCMAKE_INSTALL_PREFIX=${LIBMOSQUITTO_INSTALL_DIR}
)

if(NOT TARGET vendored-mosquitto)
  ExternalProject_Add(
    vendored-mosquitto
    URL ${PROJECT_SOURCE_DIR}/vendor/libs/mosquitto
    PATCH_COMMAND ${PATCH_COMMANDS}
    CMAKE_ARGS ${LIBMOSQUITTO_CMAKE_ARGS}
    INSTALL_DIR "${LIBMOSQUITTO_INSTALL_DIR}"
  )
  add_dependencies(vendored-mosquitto OpenSSL::SSL)
endif()

# The include dir must exist at configure time
file(MAKE_DIRECTORY ${LIBMOSQUITTO_INCLUDE_DIR})

if(MSVC)
  set(LIBMOSQUITTO_LIBRARY_PATH ${LIBMOSQUITTO_INSTALL_DIR}/lib/mosquitto_static.lib)
else()
  set(LIBMOSQUITTO_LIBRARY_PATH ${LIBMOSQUITTO_INSTALL_DIR}/lib/libmosquitto_static.a)
endif()

add_library(libmosquitto STATIC IMPORTED)
add_dependencies(libmosquitto vendored-mosquitto)
set_target_properties(
  libmosquitto
  PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  IMPORTED_LOCATION ${LIBMOSQUITTO_LIBRARY_PATH}
  INTERFACE_INCLUDE_DIRECTORIES ${LIBMOSQUITTO_INCLUDE_DIR}
  INTERFACE_COMPILE_DEFINITIONS LIBMOSQUITTO_STATIC
)
target_link_libraries(libmosquitto INTERFACE OpenSSL::SSL)

find_package_handle_standard_args(
  libmosquitto
  REQUIRED_VARS
    LIBMOSQUITTO_INSTALL_DIR
    LIBMOSQUITTO_INCLUDE_DIR
)
