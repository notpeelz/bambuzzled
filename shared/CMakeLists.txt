set(Boost_USE_STATIC_LIBS ON)
find_package(
  Boost
  1.87
  COMPONENTS filesystem
  REQUIRED
)

add_library(bambuzzled-shared INTERFACE)
set_target_properties(
  bambuzzled-shared
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(
  bambuzzled-shared
  INTERFACE
  Boost::filesystem
)
