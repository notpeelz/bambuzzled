include(FindPackageHandleStandardArgs)
include(ExternalProject)

if(MOSQUITTO_USE_STATIC_LIBS)
  # TODO: are there any Linux distros that ship static libs? Maybe on Windows?
  message(FATAL_ERROR "Static linking to system mosquitto is not supported")
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(
  LIBMOSQUITTO
  REQUIRED
  IMPORTED_TARGET
  libmosquitto
)

if(LIBMOSQUITTO_FOUND)
  add_library(mosquitto::libmosquitto INTERFACE IMPORTED)
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

  set(MOSQUITTO_INCLUDE_DIR "${LIBMOSQUITTO_INCLUDE_DIRS}")
endif()

find_package_handle_standard_args(
  mosquitto
  REQUIRED_VARS
    MOSQUITTO_INCLUDE_DIR
)
