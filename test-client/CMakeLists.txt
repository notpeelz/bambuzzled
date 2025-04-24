add_executable(
  bambuzzled-test-client
  test.cpp
)

target_link_libraries(
  bambuzzled-test-client
  PRIVATE
  OpenSSL::SSL
  libmosquitto
)
