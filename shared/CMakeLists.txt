find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

set(
  BBZL_SHARED_SOURCES
  bambuzzled/log.cpp
  bambuzzled/ssl.cpp
  bambuzzled/curl.cpp
  bambuzzled/quote.cpp
  bambuzzled/demangle.cpp
  bambuzzled/socket.cpp
  bambuzzled/socket_encrypted.cpp
)

if(WIN32)
  list(
    APPEND BBZL_SHARED_SOURCES
    bambuzzled/win32.cpp
  )
endif()

add_library(
  bambuzzled-shared
  STATIC
  ${BBZL_SHARED_SOURCES}
)
add_library(bbzl::shared ALIAS bambuzzled-shared)
add_dependencies(
  bambuzzled-shared
  CURL::libcurl
)
set_target_properties(
  bambuzzled-shared
  PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)
target_compile_definitions(
  bambuzzled-shared
  PRIVATE
  BBZL_CERTS_DIR="${CMAKE_INSTALL_PREFIX}/share/bambuzzled/certs"
  BBZL_DEFAULT_LOG_LEVEL=${BBZL_DEFAULT_LOG_LEVEL}
)
target_include_directories(
  bambuzzled-shared
  SYSTEM
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${CURL_INCLUDE_DIRS}
)
