DCEL::Builder
=============


O::DCEL::Builder::From_GeoJSON
------------------------------

.. doxygenclass:: O::DCEL::Builder::From_GeoJSON
	:members:
	:protected-members:
	:private-members:
	:undoc-members:

Usage Example
-------------

Below is a minimal example of how to use the 

.. code-block:: cpp

	#include <io/feature_Parser.h>
	#include <dcel/builder.h>

	class Auto_Builder : public O::DCEL::Builder::From_GeoJSON, public O::GeoJSON::IO::Feature_Parser<O::DCEL::Builder::From_GeoJSON> {};

	int main()
	{
		Auto_Builder auto_builder;
		rapidjson::StringStream ss("Some JSON value");
		bool ok = reader.Parse(ss, auto_builder);

		auto opt_dcel = auto_builder.Get_Dcel();
		auto opt_feature_infr = auto_builder.Get_Feature_Info(); 
	}