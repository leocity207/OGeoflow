#////////////////////////
# For google test includes
include(GoogleTest)

#/////////////
# SOURCE FILES
file(GLOB IO_TEST_SRC "./*.cpp")

#///////////
# EXECUTABLE
add_executable( io_test 
	${IO_TEST_SRC}
)

#////////////////
# EXECUTABLE LINK
target_link_libraries(io_test
	PRIVATE GTest::gtest_main
	PRIVATE io
	PRIVATE RapidJSON::rapidjson
)

#/////////////
# DECLARE TEST
gtest_discover_tests(io_test)