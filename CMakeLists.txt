cmake_minimum_required(VERSION 4.0.1)

project(
  bambuzzled
  LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(WIN32)
	add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

add_subdirectory(vendor)
add_subdirectory(shared)
add_subdirectory(src)
add_subdirectory(video)
add_subdirectory(forwarder)
add_subdirectory(test-client)
