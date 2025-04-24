add_library(bambuzzled-shared INTERFACE)
set_property(
  TARGET bambuzzled-shared
  PROPERTY
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
)
