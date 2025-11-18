include(GoogleTest)

file(GLOB FILTER_TEST_SRC "./*.cpp")

add_executable( filter_test 
	${FILTER_TEST_SRC}
)

target_include_directories(filter_test
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_libraries(filter_test
	PRIVATE GTest::gtest_main
	PRIVATE ${PROJECT_NAME}::io
	PRIVATE RapidJSON::rapidjson
)

gtest_discover_tests(filter_test)