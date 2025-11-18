#//////////////////////////////////
# SOURCE FILES
FILE(GLOB DCEL_SRC "./*.cpp")

#////////
# LIBRARY
add_library(dcel STATIC
	${DCEL_SRC}
)

#//////////////
# LIBRARY LINKS
target_link_libraries(dcel
	PRIVATE io
)

