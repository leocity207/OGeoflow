cmake_minimum_required(VERSION 3.15)
project(GeoFlow VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(cmake/compiler_option.cmake)
include(cmake/rapidjson.cmake)

#------------------------------------
# subdirectory include
add_subdirectory(src/io)
add_subdirectory(src/dcel)


#------------------------------------
# GoogleTest integration (optional)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
	include(cmake/testing.cmake)
endif()