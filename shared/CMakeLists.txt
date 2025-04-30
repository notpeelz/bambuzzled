find_package(
  Boost
  1.87
  COMPONENTS filesystem
  REQUIRED
)

add_library(
  bambuzzled-shared
  STATIC
  bambuzzled/utils/quote.cpp
  bambuzzled/utils/demangle.cpp
)
target_include_directories(
  bambuzzled-shared
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(
  bambuzzled-shared
  INTERFACE
  Boost::filesystem
)
