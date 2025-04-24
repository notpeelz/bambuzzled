add_library(
  bambuzzled
  SHARED
  bambuzzled.cpp
  stubs.cpp
  network_agent.cpp
  mosquittopp.cpp
)

set_target_properties(
  bambuzzled
  PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)

target_link_libraries(
  bambuzzled
  PRIVATE
  bambuzzled-shared
  OpenSSL::SSL
  libmosquitto
  nlohmann-json
)
