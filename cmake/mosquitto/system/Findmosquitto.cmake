include(FindPackageHandleStandardArgs)
include(ExternalProject)

find_package(PkgConfig REQUIRED QUIET)
if(PKGCONFIG_FOUND)
  pkg_check_modules(
    LIBMOSQUITTO
    REQUIRED
    IMPORTED_TARGET
    libmosquitto
    QUIET
  )
endif()

if(LIBMOSQUITTO_FOUND)
  add_library(mosquitto::libmosquitto UNKNOWN IMPORTED)
  target_include_directories(
    mosquitto::libmosquitto
    INTERFACE
    ${LIBMOSQUITTO_INCLUDE_DIRS}
  )
  target_link_libraries(
    mosquitto::libmosquitto
    INTERFACE
    ${LIBMOSQUITTO_LINK_LIBRARIES}
  )

  set(MOSQUITTO_LIBRARIES ${LIBMOSQUITTO_LINK_LIBRARIES})
  set(MOSQUITTO_INCLUDE_DIRS ${LIBMOSQUITTO_INCLUDE_DIRS})
endif()

find_package_handle_standard_args(
  mosquitto
  REQUIRED_VARS
    MOSQUITTO_LIBRARIES
    MOSQUITTO_INCLUDE_DIRS
)
