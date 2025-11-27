.. _feature_filter:

O::GeoJSON::Filter::Feature
===========================

Technical documentation
-----------------------

.. doxygenclass:: O::GeoJSON::Filter::Feature
	:members:
	:protected-members:
	:private-members:
	:undoc-members:

Usage Example
-------------

Below is a minimal example of how to use the Full_Parser


.. code-block:: cpp

	#include <filter/feature.h>

	struct Pred_Always_True { 
		bool operator()(const Feature&){return true; }
	}

	struct Feature_Collector
	{
		bool On_Full_Feature(Feature&& f)
		{
			// Do things
		}

		bool On_Root(std::optional<Bbox>&& bbox, std::optional<std::string>&& id)
		{
			// Do things
		}
	};

	O::GeoJSON::Filter::Feature<Feature_Collector, Pred_Always_True> filtered_parser;
	rapidjson::StringStream ss("Some JSON value");
	bool ok = reader.Parse(ss, filtered_parser);

See Also
--------

* :ref:`feature_parser`