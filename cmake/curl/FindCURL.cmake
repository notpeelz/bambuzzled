include(FindPackageHandleStandardArgs)
include(ExternalProject)

set(CURL_INSTALL_DIR ${CMAKE_BINARY_DIR}/libs/install/libcurl)
set(CURL_INCLUDE_DIR ${CURL_INSTALL_DIR}/include)

find_package(OpenSSL REQUIRED)

if(MSVC)
  set(CURL_LIBRARY_STATIC_FILE libcurl.lib)
  set(CURL_LIBRARY_SHARED_FILE libcurl_imp.lib)
else()
  set(CURL_LIBRARY_STATIC_FILE libcurl.a)
  set(CURL_LIBRARY_SHARED_FILE libcurl.so)
endif()

set(
  CURL_DEPENDENCIES
  OpenSSL::SSL
)

if(NOT TARGET vendored-curl)
  set(CURL_CMAKE_ARGS "")

  # TODO: enable ARES
  list(
    APPEND CURL_CMAKE_ARGS
    -DBUILD_STATIC_LIBS=ON
    -DBUILD_SHARED_LIBS=ON
    -DBUILD_CURL_EXE=OFF
    -DCURL_USE_PKGCONFIG=OFF
    -DENABLE_ARES=OFF
    -DCURL_ZLIB=OFF
    -DCURL_ZSTD=OFF
    -DCURL_BROTLI=OFF
    -DCURL_USE_LIBPSL=OFF
    -DCURL_USE_LIBSSH2=OFF
    -DUSE_NGHTTP2=OFF
    -DUSE_LIBIDN2=OFF
    -DCURL_USE_OPENSSL=OFF
    -DCURL_DISABLE_LDAP=ON
    -DCURL_DISABLE_NETRC=ON
    -DCMAKE_INSTALL_PREFIX=${CURL_INSTALL_DIR}
  )

  if(DEFINED CMAKE_BUILD_TYPE)
    list(
      APPEND CURL_CMAKE_ARGS
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    )
  endif()

  if(DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
    list(
      APPEND CURL_CMAKE_ARGS
      -DCMAKE_MSVC_RUNTIME_LIBRARY=${CMAKE_MSVC_RUNTIME_LIBRARY}
    )
  endif()

  if(DEFINED OPENSSL_ROOT_DIR)
    list(
      APPEND CURL_CMAKE_ARGS
      -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR}
    )
  endif()

  ExternalProject_Add(
    vendored-curl
    PREFIX ${CMAKE_BINARY_DIR}/libs/prefix/libcurl
    URL ${PROJECT_SOURCE_DIR}/libs/curl
    CMAKE_ARGS ${CURL_CMAKE_ARGS}
    INSTALL_DIR ${CURL_INSTALL_DIR}
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config $<CONFIG>
    INSTALL_COMMAND ${CMAKE_COMMAND} --install <BINARY_DIR> --config $<CONFIG>
    BUILD_BYPRODUCTS
      ${CURL_INSTALL_DIR}/lib/${CURL_LIBRARY_STATIC_FILE}
      ${CURL_INSTALL_DIR}/lib/${CURL_LIBRARY_SHARED_FILE}
  )
  add_dependencies(vendored-curl ${CURL_DEPENDENCIES})
endif()

# The include dir must exist at configure time
file(MAKE_DIRECTORY ${CURL_INCLUDE_DIR})

if(CURL_USE_STATIC_LIBS)
  set(
    CURL_LIBRARY_PATH
    ${CURL_INSTALL_DIR}/lib/${CURL_LIBRARY_STATIC_FILE}
  )
else()
  set(
    CURL_LIBRARY_PATH
    ${CURL_INSTALL_DIR}/lib/${CURL_LIBRARY_SHARED_FILE}
  )
endif()

add_library(CURL::libcurl STATIC IMPORTED)
add_dependencies(CURL::libcurl vendored-curl)
set_target_properties(
  CURL::libcurl
  PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  IMPORTED_LOCATION ${CURL_LIBRARY_PATH}
)
target_compile_definitions(
  CURL::libcurl
  INTERFACE
  CURL_STATICLIB
)
target_include_directories(
  CURL::libcurl
  INTERFACE
  ${CURL_INCLUDE_DIR}
)
target_link_libraries(
  CURL::libcurl
  INTERFACE
  ${CURL_DEPENDENCIES}
)

set(
  CURL_LIBRARIES
  ${CURL_LIBRARY_PATH}
  ${CURL_DEPENDENCIES}
)
set(
  CURL_INCLUDE_DIRS
  ${CURL_INCLUDE_DIR}
)

find_package_handle_standard_args(
  CURL
  REQUIRED_VARS
    CURL_LIBRARIES
    CURL_INCLUDE_DIRS
)
