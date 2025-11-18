
include(GoogleTest)


file(GLOB IO_TEST_SRC "./*.cpp")


add_executable( io_test 
	${IO_TEST_SRC}
)

target_include_directories(io_test
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_libraries(io_test
	PRIVATE GTest::gtest_main
	PRIVATE ${PROJECT_NAME}::io
	PRIVATE RapidJSON::rapidjson
)

gtest_discover_tests(io_test)