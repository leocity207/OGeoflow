include(CTest)
include(cmake/googletest.cmake)

add_subdirectory(src/io/test)
add_subdirectory(src/filter/test)
add_subdirectory(src/dcel/test)

enable_testing()