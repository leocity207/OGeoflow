file(GLOB IO_SRC "./*.cpp")

add_library(io STATIC ${IO_SRC})

target_include_directories(io
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${PROJECT_SOURCE_DIR}
)

target_link_libraries(io
	PUBLIC RapidJSON::rapidjson
	PUBLIC OConfigurator::configuration
)

add_library(${PROJECT_NAME}::io ALIAS io)
