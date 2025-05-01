find_package(OpenSSL REQUIRED)
find_package(mosquitto REQUIRED)
find_package(nlohmann_json REQUIRED)

add_library(
  bambuzzled-network-plugin
  SHARED
  bambuzzled.cpp
  stubs.cpp
  network_agent.cpp
  mosquittopp.cpp
)
add_library(bbzl::network-plugin ALIAS bambuzzled-network-plugin)
target_compile_definitions(
  bambuzzled-network-plugin
  PRIVATE
  BBZL_NETWORK_PLUGIN_EXPORT
)
set_target_properties(
  bambuzzled-network-plugin
  PROPERTIES
  OUTPUT_NAME bambu_networking
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)
target_link_libraries(
  bambuzzled-network-plugin
  PRIVATE
  bbzl::shared
  OpenSSL::SSL
  mosquitto::libmosquitto
  nlohmann_json::nlohmann_json
)
