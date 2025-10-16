#//////////////////////////////////
# SOURCE FILES
FILE(GLOB GEOJSON_SRC "./*.cpp")

#////////
# LIBRARY
add_library(geojson STATIC
	${GEOJSON_SRC}
)

#//////////////
# LIBRARY LINKS
target_link_libraries(geojson)