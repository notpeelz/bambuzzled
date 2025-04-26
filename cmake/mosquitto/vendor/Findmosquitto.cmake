include(FindPackageHandleStandardArgs)
include(ExternalProject)

set(MOSQUITTO_INSTALL_DIR ${CMAKE_BINARY_DIR}/libs/install/libmosquitto)
set(MOSQUITTO_INCLUDE_DIR ${MOSQUITTO_INSTALL_DIR}/include)

find_package(OpenSSL REQUIRED)

if(MSVC)
  set(MOSQUITTO_LIBRARY_STATIC_FILE mosquitto_static.lib)
  set(MOSQUITTO_LIBRARY_SHARED_FILE mosquitto.lib)
else()
  set(MOSQUITTO_LIBRARY_STATIC_FILE libmosquitto_static.a)
  set(MOSQUITTO_LIBRARY_SHARED_FILE libmosquitto.so)
endif()

set(
  MOSQUITTO_DEPENDENCIES
  OpenSSL::SSL
)

if(NOT TARGET vendored-mosquitto)
  set(MOSQUITTO_CMAKE_ARGS "")

  list(
    APPEND MOSQUITTO_CMAKE_ARGS
    -DWITH_STATIC_LIBRARIES=ON
    -DWITH_PIC=ON
    -DWITH_THREADING=OFF
    -DWITH_CJSON=OFF
    -DWITH_CLIENTS=OFF
    -DWITH_BROKER=OFF
    -DWITH_APPS=OFF
    -DWITH_PLUGINS=OFF
    -DDOCUMENTATION=OFF
    -DCMAKE_INSTALL_PREFIX=${MOSQUITTO_INSTALL_DIR}
  )

  if(DEFINED CMAKE_BUILD_TYPE)
    list(
      APPEND MOSQUITTO_CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    )
  endif()

  if(DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    list(
      APPEND MOSQUITTO_CMAKE_ARGS
      -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
    )
  endif()

  if(DEFINED OPENSSL_ROOT_DIR)
    list(
      APPEND MOSQUITTO_CMAKE_ARGS
      -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
    )
  endif()

  ExternalProject_Add(
    vendored-mosquitto
    PREFIX ${CMAKE_BINARY_DIR}/libs/prefix/libmosquitto
    URL ${PROJECT_SOURCE_DIR}/libs/mosquitto
    CMAKE_ARGS ${MOSQUITTO_CMAKE_ARGS}
    INSTALL_DIR ${MOSQUITTO_INSTALL_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>
    BUILD_BYPRODUCTS
      ${MOSQUITTO_INSTALL_DIR}/lib/${MOSQUITTO_LIBRARY_STATIC_FILE}
      ${MOSQUITTO_INSTALL_DIR}/lib/${MOSQUITTO_LIBRARY_SHARED_FILE}
  )
  add_dependencies(vendored-mosquitto ${MOSQUITTO_DEPENDENCIES})
endif()

# The include dir must exist at configure time
file(MAKE_DIRECTORY ${MOSQUITTO_INCLUDE_DIR})

if(MOSQUITTO_USE_STATIC_LIBS)
  set(
    MOSQUITTO_LIBRARY_PATH
    ${MOSQUITTO_INSTALL_DIR}/lib/${MOSQUITTO_LIBRARY_STATIC_FILE}
  )
else()
  set(
    MOSQUITTO_LIBRARY_PATH
    ${MOSQUITTO_INSTALL_DIR}/lib/${MOSQUITTO_LIBRARY_SHARED_FILE}
  )
endif()

add_library(mosquitto::libmosquitto STATIC IMPORTED)
add_dependencies(mosquitto::libmosquitto vendored-mosquitto)
set_target_properties(
  mosquitto::libmosquitto
  PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  IMPORTED_LOCATION ${MOSQUITTO_LIBRARY_PATH}
)
target_compile_definitions(
  mosquitto::libmosquitto
  INTERFACE
  LIBMOSQUITTO_STATIC
)
target_include_directories(
  mosquitto::libmosquitto
  INTERFACE
  ${MOSQUITTO_INCLUDE_DIR}
)
target_link_libraries(
  mosquitto::libmosquitto
  INTERFACE
  ${MOSQUITTO_DEPENDENCIES}
)

set(
  MOSQUITTO_LIBRARIES
  ${MOSQUITTO_LIBRARY_PATH}
  ${MOSQUITTO_DEPENDENCIES}
)
set(
  MOSQUITTO_INCLUDE_DIRS
  ${MOSQUITTO_INCLUDE_DIR}
)

find_package_handle_standard_args(
  mosquitto
  REQUIRED_VARS
    MOSQUITTO_LIBRARIES
    MOSQUITTO_INCLUDE_DIRS
)
