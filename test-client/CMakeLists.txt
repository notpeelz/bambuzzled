find_package(OpenSSL REQUIRED)
find_package(libmosquitto REQUIRED)

add_executable(
  bambuzzled-test-client
  test.cpp
)
set_target_properties(
  bambuzzled-test-client
  PROPERTIES
  OUTPUT_NAME test-client
)
target_link_libraries(
  bambuzzled-test-client
  PRIVATE
  OpenSSL::SSL
  libmosquitto
)
