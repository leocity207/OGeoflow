O::GeoJSON::IO
==============

The ``O::GeoJSON::IO`` namespace concerns everything related to input and output.

This implementation provides:

* Two SAX parsers:
  * A SAX parser handling Geometry, Feature, and FeatureCollection
  * A SAX parser handling Feature only
* One full parser
* One writer

.. toctree::
	:maxdepth: 2

	sax_parser
	feature_parser
	full_parser
	writer
