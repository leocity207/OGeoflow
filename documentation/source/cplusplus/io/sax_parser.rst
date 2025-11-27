.. _sax_parser:

O::GeoJSON::IO::Sax_Parser
==========================

implementation
--------------

.. doxygenclass:: O::GeoJSON::IO::SAX_Parser
	:members:
	:protected-members:
	:undoc-members:

Required Interface Functions
----------------------------

someone that want to implement a :cpp:class:`O::GeoJSON::IO::SAX_Parser` must also implement the following functions:

	- `bool On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t element_number)`
	- `bool On_Feature(O::GeoJSON::Feature&& feature)`
	- `bool On_Feature_Collection(std::optional<Bbox>&& bbox, std::optional<std::string>&& id)`


Workflow Diagrams
-----------------

The RapidJSON SAX interface triggers callbacks in a non-trivial order.
To assist developers that want to understand the flow of event inside the SAX parser, the parsing workflow for each handler is documented:

.. graphviz:: ../../../diagram/startobject.dot
	:align: center
	:caption: **StartObject() workflow**
	
.. graphviz:: ../../../diagram/endobject.dot
	:align: center
	:caption: **EndObject() workflow**

.. graphviz:: ../../../diagram/startarray.dot
	:align: center
	:caption: **StartArray() workflow**

.. graphviz:: ../../../diagram/endarray.dot
	:align: center
	:caption: **EndArray() workflow**

.. graphviz:: ../../../diagram/key.dot
	:align: center
	:caption: **EndArray() workflow**

.. graphviz:: ../../../diagram/bool.dot
	:align: center
	:caption: **Bool() workflow**

.. graphviz:: ../../../diagram/int_double.dot
	:align: center
	:caption: **Int() Double() workflow**

.. graphviz:: ../../../diagram/null.dot
	:align: center
	:caption: **Null() workflow**

.. graphviz:: ../../../diagram/string.dot
	:align: center
	:caption: **String() workflow**

Usage Example
-------------

A user wanting to process GeoJSON streams must extend the parser:

.. code-block:: cpp

	class MyParser : public O::GeoJSON::IO::SAX_Parser<MyParser>
	{
	public:
		bool On_Geometry(O::GeoJSON::Geometry&& geom, std::size_t index) override
		{
			// Handle geometry
			return true;
		}

		bool On_Feature(O::GeoJSON::Feature&& feature) override
		{
			// Handle feature
			return true;
		}

		bool On_Feature_Collection(std::optional<Bbox>&& bbox, std::optional<std::string>&& id) override
		{
			// Finalize collection
			return true;
		}
	};

	int main()
	{
		rapidjson::StringStream ss("...");
		rapidjson::Reader reader;
		MyParser parser;
		bool ok = reader.Parse(ss, parser);
	}

See Also
--------

- :ref:`geojson_root`
- :ref:`geojson_geometry`
- :ref:`geojson_objects`
- :ref:`geojson_decorations`
