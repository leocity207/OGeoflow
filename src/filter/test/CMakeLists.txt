include(../../../cmake/add_simple_test.cmake)
Add_Simple_Googletest_Target(filter_test
	${PROJECT_NAME}::io
	RapidJSON::rapidjson
)