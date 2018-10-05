cmake_minimum_required(VERSION 2.8)

project(inception-rtdebugger-driver)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set (CMAKE_CXX_STANDARD 11)

include_directories("include")

add_subdirectory(lib)
add_subdirectory(tools)

# add_library(inception-dbg-cli_stl STATIC ${SOURCES})
# add_library(inception-dbg-cli_shl SHARED ${SOURCES})
