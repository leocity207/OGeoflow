#//////////////////////////////////
# SOURCE FILES
FILE(GLOB IO_SRC "./*.cpp")

#////////
# LIBRARY
add_library(io STATIC
	${IO_SRC}
)

#//////////////
# LIBRARY LINKS
target_link_libraries(io
	PRIVATE RapidJSON::rapidjson
)

