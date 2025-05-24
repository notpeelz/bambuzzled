add_library(
  bambuzzled-forwarder
  SHARED
  init.cpp
  forwarder.cpp
)
add_library(bbzl::forwarder ALIAS bambuzzled-forwarder)
target_compile_definitions(
  bambuzzled-forwarder
  PRIVATE
  BBZL_NETWORK_PLUGIN_EXPORT
)
set_target_properties(
  bambuzzled-forwarder
  PROPERTIES
  OUTPUT_NAME bbzl-forwarder
  DEFINE_SYMBOL ""
  CXX_VISIBILITY_PRESET hidden
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)
target_link_libraries(
  bambuzzled-forwarder
  PRIVATE
  bbzl::shared
)
install(
  TARGETS bambuzzled-forwarder
  DESTINATION lib/bambuzzled
)
