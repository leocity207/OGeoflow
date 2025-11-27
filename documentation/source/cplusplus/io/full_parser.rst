.. _full_parser:

O::GeoJSON::IO::Full_Parser
===========================

Technical documentation
-----------------------

.. doxygenclass:: O::GeoJSON::IO::Full_Parser
	:members:
	:protected-members:
	:private-members:
	:undoc-members:

Usage Example
-------------

Below is a minimal example of how to use the Full_Parser


.. code-block:: cpp

	#include <io/full_parser.h>
	#include <rapidjson/document.h>

	rapidjson::Reader reader;
	O::GeoJSON::IO::Full_Parser handler;
	
	rapidjson::StringStream ss("A valid JSON string");
	
	if (!reader.Parse(ss, handler) )
	{
		// handle parsing error
	}
	if (auto geojson = handler.Get_Geojson())
	{
		//get the geoJSON
	}