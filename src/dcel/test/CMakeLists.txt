include(../../../cmake/add_simple_test.cmake)
Add_Simple_Googletest_Target(dcel_test
	${PROJECT_NAME}::dcel
	${PROJECT_NAME}::io
	RapidJSON::rapidjson
	OUtils::utils
)