cmake_minimum_required(VERSION 3.15)
project(OGeoFlow VERSION 0.0.6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

#------
# setup
include(cmake/compiler_warning.cmake)
include(cmake/simple_fetch_repository.cmake)
include(cmake/simple_fetch_populate.cmake)
Simple_Fetch_Populate(
	rapdijson
	https://github.com/Tencent/rapidjson.git
	v1.1.0
	RapidJSON::rapidjson
)
Simple_Fetch_Repository(
	OUtils
	https://github.com/leocity207/OUtils.git
	V0.0.4
)
Simple_Fetch_Repository(
	OConfigurator
	https://github.com/leocity207/OConfigurator.git
	V0.0.4
)

#------------------------------------
# subdirectory include
add_subdirectory(src/io)
add_subdirectory(src/dcel)


#------------------------------------
# GoogleTest integration (optional)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
	include(CTest)
	add_subdirectory(src/dcel/test)
	add_subdirectory(src/filter/test)
	add_subdirectory(src/io/test)
endif()

option(BUILD_DOC "Build doc" ON)
if(BUILD_DOC)
	add_subdirectory(documentation)
endif()