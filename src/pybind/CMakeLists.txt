#//////////////////////////////////
# SOURCE FILES
FILE(GLOB PYBIND_SRC "./*.cpp")


# Module Python
pybind11_add_module(pygeoflow
	${PYBIND_SRC}
)

#incldue
target_link_libraries(pygeoflow PRIVATE 
	pybind11::module
	io
	dcel
	dcel_configuration
)

# Compiler options
set_target_properties(pygeoflow PROPERTIES
	CXX_STANDARD 20
	OUTPUT_NAME "pygeoflow"
	SUFFIX ".pyd"
)