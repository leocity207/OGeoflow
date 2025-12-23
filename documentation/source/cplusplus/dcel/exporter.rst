DCEL::Exporter
==============

O::DCEL::Exporter::To_GeoJSON
-----------------------------

.. doxygenclass:: O::DCEL::Exporter::To_GeoJSON
	:members:
	:protected-members:
	:private-members:
	:undoc-members:

Usage Example
-------------

Below is a minimal example of how to use the O::DCEL::Exporter

.. code-block:: cpp

	#include <dcel/exporter.h>

	int main()
	{
		O::DCEL::Storage dcel = ...
		O::DCEL::Feature_Info info = ...;
		O::GeoJSON::Root out = O::DCEL::Exporter::To_GeoJSON::Convert(dcel, info);
	}