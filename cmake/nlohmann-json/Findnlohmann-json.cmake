include(FindPackageHandleStandardArgs)
include(ExternalProject)

set(NLOHMANN_JSON_INSTALL_DIR ${CMAKE_BINARY_DIR}/vendor/nlohmann-json)
set(NLOHMANN_JSON_INCLUDE_DIR ${NLOHMANN_JSON_INSTALL_DIR}/include)

if(NOT TARGET vendored-nlohmann-json)
  ExternalProject_Add(
    vendored-nlohmann-json
    URL ${PROJECT_SOURCE_DIR}/vendor/libs/nlohmann-json
    CMAKE_ARGS
      -DJSON_BuildTests=OFF
      -DCMAKE_INSTALL_PREFIX=${NLOHMANN_JSON_INSTALL_DIR}
    INSTALL_DIR ${NLOHMANN_JSON_INSTALL_DIR}
  )
endif()

file(MAKE_DIRECTORY ${NLOHMANN_JSON_INCLUDE_DIR})

add_library(nlohmann-json INTERFACE IMPORTED GLOBAL)
set_target_properties(
  nlohmann-json
  PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${NLOHMANN_JSON_INCLUDE_DIR}
)
add_dependencies(nlohmann-json vendored-nlohmann-json)

find_package_handle_standard_args(
  nlohmann-json
  REQUIRED_VARS
    NLOHMANN_JSON_INSTALL_DIR
    NLOHMANN_JSON_INCLUDE_DIR
)
