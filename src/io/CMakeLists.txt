include(../../cmake/add_simple_library.cmake)
Add_Simple_Library(io 
	STATIC 
	
	RapidJSON::rapidjson 
	OConfigurator::configuration
)