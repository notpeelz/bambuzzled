include(ExternalProject)

set(OPENSSL_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/install)
set(OPENSSL_INSTALL_DIR ${OPENSSL_INSTALL_DIR} PARENT_SCOPE)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_INSTALL_DIR}/include)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_INCLUDE_DIR} PARENT_SCOPE)

if(WIN32)
  # unfortunately openssl (with nmake) doesn't support parallel building (doesn't work with jom either)
  set(OPENSSL_BUILD_COMMAND nmake)
  set(OPENSSL_INSTALL_COMMAND nmake install)
else()
  set(OPENSSL_BUILD_COMMAND make -j${CMAKE_BUILD_PARALLEL_LEVEL})
  set(OPENSSL_INSTALL_COMMAND make install)
endif()

ExternalProject_Add(
  openssl-project
  BUILD_IN_SOURCE TRUE
  URL ${CMAKE_CURRENT_SOURCE_DIR}/../libs/openssl
  CONFIGURE_COMMAND perl Configure
    no-docs
    no-unit-test
    --prefix=${OPENSSL_INSTALL_DIR}
    --libdir=lib
    --openssldir=${OPENSSL_INSTALL_DIR}
    -static
    -fPIC
  BUILD_COMMAND ${OPENSSL_BUILD_COMMAND}
  INSTALL_COMMAND ${OPENSSL_INSTALL_COMMAND}
  INSTALL_DIR ${OPENSSL_INSTALL_DIR}
)

file(MAKE_DIRECTORY ${OPENSSL_INCLUDE_DIR})

if(MSVC)
  set(OPENSSL_SSL_LIBRARY_PATH ${OPENSSL_INSTALL_DIR}/lib/libssl.lib)
  set(OPENSSL_CRYPTO_LIBRARY_PATH ${OPENSSL_INSTALL_DIR}/lib/libcrypto.lib)
else()
  set(OPENSSL_SSL_LIBRARY_PATH ${OPENSSL_INSTALL_DIR}/lib/libssl.a)
  set(OPENSSL_CRYPTO_LIBRARY_PATH ${OPENSSL_INSTALL_DIR}/lib/libcrypto.a)
endif()

add_library(OpenSSL INTERFACE)
add_dependencies(OpenSSL openssl-project)

add_library(OpenSSL::SSL STATIC IMPORTED GLOBAL)
set_target_properties(
  OpenSSL::SSL
  PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  IMPORTED_LOCATION ${OPENSSL_SSL_LIBRARY_PATH}
  INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR}
)

add_library(OpenSSL::Crypto STATIC IMPORTED GLOBAL)
set_target_properties(
  OpenSSL::Crypto
  PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  IMPORTED_LOCATION ${OPENSSL_CRYPTO_LIBRARY_PATH}
  INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR}
)

add_dependencies(OpenSSL::Crypto OpenSSL)
add_dependencies(OpenSSL::SSL OpenSSL OpenSSL::Crypto)

if(WIN32)
  find_library(ZLIB_LIBRARY zlib REQUIRED)
  target_link_libraries(
    OpenSSL::SSL
    INTERFACE
    OpenSSL::Crypto
  )
  target_link_libraries(
    OpenSSL::Crypto
    INTERFACE
    ${ZLIB_LIBRARY}
    ws2_32.lib
    gdi32.lib
    advapi32.lib
    crypt32.lib
    user32.lib
  )
elseif(LINUX)
  target_link_libraries(
    OpenSSL::SSL
    INTERFACE
    OpenSSL::Crypto
    dl
    pthread
  )
  target_link_libraries(
    OpenSSL::Crypto
    INTERFACE
    dl
    pthread
  )
else()
  message(FATAL_ERROR "Unsupported platform")
endif()
