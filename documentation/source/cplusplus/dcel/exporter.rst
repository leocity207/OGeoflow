DCEL::Exporter
==============

DCEL::Builder
=============

O::DCEL::Exporter
-----------------

.. doxygenclass:: O::DCEL::Exporter
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
		O::GeoJSON::Root out = O::DCEL::Exporter::Convert(dcel, info);
	}