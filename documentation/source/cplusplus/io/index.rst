O::GeoJSON::IO
==============

GeoJSON::IO namespace concern everything about Input and output. 
This implementation. choose to create:
	* two sax Parser 
		* Sax Parser with Geometry Feature Feature_Collection
		* Sax Parser with Feature Only
	* one full Parser
	* one writer

.. toctree::
	:maxdepth: 2

	sax_parser
	feature_parser
	full_parser
	writer
