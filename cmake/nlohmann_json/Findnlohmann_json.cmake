include(FindPackageHandleStandardArgs)
include(ExternalProject)

set(NLOHMANN_JSON_INSTALL_DIR ${CMAKE_BINARY_DIR}/libs/install/nlohmann-json)
set(NLOHMANN_JSON_INCLUDE_DIR ${NLOHMANN_JSON_INSTALL_DIR}/include)

if(NOT TARGET vendored-nlohmann-json)
  ExternalProject_Add(
    vendored-nlohmann-json
    PREFIX ${CMAKE_BINARY_DIR}/libs/prefix/nlohmann-json
    URL ${PROJECT_SOURCE_DIR}/libs/nlohmann-json
    CMAKE_ARGS
      -DJSON_BuildTests=OFF
      -DCMAKE_INSTALL_PREFIX=${NLOHMANN_JSON_INSTALL_DIR}
    INSTALL_DIR ${NLOHMANN_JSON_INSTALL_DIR}
  )
endif()

file(MAKE_DIRECTORY ${NLOHMANN_JSON_INCLUDE_DIR})

add_library(nlohmann_json::nlohmann_json INTERFACE IMPORTED GLOBAL)
target_include_directories(
  nlohmann_json::nlohmann_json
  INTERFACE
  ${NLOHMANN_JSON_INCLUDE_DIR}
)
add_dependencies(nlohmann_json::nlohmann_json vendored-nlohmann-json)

find_package_handle_standard_args(
  nlohmann_json
  REQUIRED_VARS
    NLOHMANN_JSON_INCLUDE_DIR
)
