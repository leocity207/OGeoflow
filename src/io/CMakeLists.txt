file(GLOB IO_SRC "./*.cpp")

add_library(io STATIC ${IO_SRC})


target_include_directories(io
    PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
      $<INSTALL_INTERFACE:include>
)


target_link_libraries(io
    PUBLIC RapidJSON::rapidjson
)


add_library(${PROJECT_NAME}::io ALIAS io)
