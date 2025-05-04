find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_library(
  bambuzzled-shared
  STATIC
  bambuzzled/log.cpp
  bambuzzled/ssl.cpp
  bambuzzled/quote.cpp
  bambuzzled/demangle.cpp
  bambuzzled/win32.cpp
)
add_library(bbzl::shared ALIAS bambuzzled-shared)
bbzl_target_link_options_no_undefined(bambuzzled-shared)
set_target_properties(
  bambuzzled-shared
  PROPERTIES
  POSITION_INDEPENDENT_CODE ON
)
target_compile_definitions(
  bambuzzled-shared
  PRIVATE
  BBZL_CERTS_DIR="${CMAKE_INSTALL_PREFIX}/share/bambuzzled/certs"
)
target_include_directories(
  bambuzzled-shared
  PUBLIC
  ${OPENSSL_INCLUDE_DIR}
  ${CURL_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
)
