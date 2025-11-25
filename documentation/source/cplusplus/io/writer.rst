.. _writer:

O::GeoJSON::IO::Writer
======================

Technical documentation
-----------------------

.. doxygenclass:: O::GeoJSON::IO::Writer
   :members:
   :protected-members:
   :private-members:
   :undoc-members:


Usage Example
-------------

Below is a minimal example of how to use the Full_Parser


.. code-block:: cpp

	#include <io/writer.h>
	#include <rapidjson/writer.h>
	#include <rapidjson/ostreamwrapper.h>

	int main()
	{
		O::GeoJSON::Root
		std::stringstream ss;
		rapidjson::OStreamWrapper osw(ss);
		O::GeoJSON::IO::Writer<rapidjson::OStreamWrapper> writer(osw);
		writer.Write_GeoJSON_Object(obj);
		return ss.str();
	}