#////////////////////////
# For google test includes
include(GoogleTest)

#/////////////
# SOURCE FILES
file(GLOB FILTER_TEST_SRC "./*.cpp")

#///////////
# EXECUTABLE
add_executable( filter_test 
	${FILTER_TEST_SRC}
)

target_include_directories(filter_test
    PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
      $<INSTALL_INTERFACE:include>
)

#////////////////
# EXECUTABLE LINK
target_link_libraries(filter_test
	PRIVATE GTest::gtest_main
	PRIVATE ${PROJECT_NAME}::io
	PRIVATE RapidJSON::rapidjson
)

#/////////////
# DECLARE TEST
gtest_discover_tests(filter_test)