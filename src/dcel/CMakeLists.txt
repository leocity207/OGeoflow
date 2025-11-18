file(GLOB DCEL_SRC "./*.cpp")

add_library(dcel STATIC ${DCEL_SRC})

target_include_directories(dcel
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_libraries(dcel
	PUBLIC ${PROJECT_NAME}::io
)

add_library(${PROJECT_NAME}::dcel ALIAS dcel)
