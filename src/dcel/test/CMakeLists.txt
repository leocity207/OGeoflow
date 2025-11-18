#////////////////////////
# For google test includes
include(GoogleTest)

#/////////////
# SOURCE FILES
file(GLOB DCEL_TEST_SRC "./*.cpp")

#///////////
# EXECUTABLE
add_executable( dcel_test 
	${DCEL_TEST_SRC}
)

#////////////////
# EXECUTABLE LINK
target_link_libraries(dcel_test
	PRIVATE GTest::gtest_main
	PRIVATE dcel
	PRIVATE RapidJSON::rapidjson
)

#/////////////
# DECLARE TEST
gtest_discover_tests(dcel_test)