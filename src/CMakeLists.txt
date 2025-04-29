find_package(OpenSSL REQUIRED)
find_package(mosquitto REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(
  bambuzzled
  SHARED
  bambuzzled.cpp
  stubs.cpp
  log.cpp
  network_agent.cpp
  mosquittopp.cpp
)
set_target_properties(
  bambuzzled
  PROPERTIES
  OUTPUT_NAME bambu_networking
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)
target_link_libraries(
  bambuzzled
  PRIVATE
  bambuzzled-shared
  OpenSSL::SSL
  mosquitto::libmosquitto
  nlohmann_json::nlohmann_json
)
