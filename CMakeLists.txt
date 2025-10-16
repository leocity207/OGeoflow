cmake_minimum_required(VERSION 3.15)
project(GeoFlow VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

#---------------------------------------
# get the current commit hash for version
execute_process(
	COMMAND git rev-parse HEAD
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	OUTPUT_VARIABLE GIT_COMMIT_HASH
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

#------------------------------------------
# compiler option
if (MSVC)
	add_compile_options(/W4 /utf-8)
	add_compile_definitions(_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
	add_compile_options(-Wall -Wextra -pedantic -Og)
endif()

#------------------------------------
# rapidjson integration
include(FetchContent)
FetchContent_Declare(
    rapidjson
    GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
    GIT_TAG v1.1.0
    UPDATE_DISCONNECTED ON
)
set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "Disable rapidjson tests" FORCE)
set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "Disable rapidjson examples" FORCE)
set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "Disable rapidjson docs" FORCE)
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF CACHE BOOL "Don't build bundled gtest" FORCE)
FetchContent_MakeAvailable(rapidjson)
add_library(RapidJSON::rapidjson INTERFACE IMPORTED)
set_target_properties(RapidJSON::rapidjson PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${rapidjson_SOURCE_DIR}/include"
)

#------------------------------------
# subdirectory include
add_subdirectory(src/geojson)
add_subdirectory(src/io)


#------------------------------------
# GoogleTest integration (optional)
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
	include(CTest)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG v1.17.0
	)
	include(GoogleTest)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	add_subdirectory(src/io/test)
	enable_testing()
endif()