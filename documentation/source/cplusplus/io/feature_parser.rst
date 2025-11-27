.. _feature_parser:

O::GeoJSON::IO::Feature_Parser
==============================

Technical Documentation
-----------------------

.. doxygenclass:: O::GeoJSON::IO::Feature_Parser
	:members:
	:protected-members:
	:private-members:
	:undoc-members:

Required Interface Functions
----------------------------

someone that want to implement a :cpp:class:`O::GeoJSON::IO::Feature_Parser` must also implement the following functions

	- `bool On_Full_Feature(O::GeoJSON::Feature&& feature)`
	- `bool On_Feature_Collection(std::optional<Bbox>&& bbox, std::optional<std::string>&& id)`

Usage Example
-------------

Below is a minimal example of how to implement a custom parser
using the CRTP mechanism:

.. code-block:: cpp

	#include <io/feature_parser.h>

	class MyParser : public O::GeoJSON::IO::Feature_Parser<MyParser>
	{
	public:
		bool On_Full_Feature(O::GeoJSON::Feature&& feature)
		{
			// Handle feature
			return true;
		}

		bool On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t index)
		{
			// Handle geometry
			return true;
		}
	};

	int main()
	{
		rapidjson::Reader reader;
		MyParser handler;
		
		rapidjson::StringStream ss("A valid JSON string");
		
		if (!reader.Parse(ss, handler) )
		{
			// handle parsing error
		}
		if (auto geojson = handler.Get_Geojson())
		{
			//get the geoJSON
		}
	}