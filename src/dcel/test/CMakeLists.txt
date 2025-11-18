
include(GoogleTest)

file(GLOB DCEL_TEST_SRC "./*.cpp")

add_executable( dcel_test 
	${DCEL_TEST_SRC}
)

target_include_directories(dcel_test
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_libraries(dcel_test
	PRIVATE GTest::gtest_main
	PRIVATE ${PROJECT_NAME}::dcel
	PRIVATE RapidJSON::rapidjson
)

gtest_discover_tests(dcel_test)