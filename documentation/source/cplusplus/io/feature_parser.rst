.. _feature_parser:

GeoJSON::IO::Feature_Parser
===========================

The :code:`Feature_Parser` class provides the SAX callback interface used
to parse GeoJSON **Feature** and **FeatureCollection** structures.
It is part of the :code:`GeoJSON::IO` namespace and is intended
to be extended through the CRTP pattern.

This component is built on top of :code:`SAX_Parser`, enabling efficient
streaming parsing without loading the entire JSON document into memory.

Overview
--------

The parser dispatches the following events:

* Geometry parsed inside a Feature
* Full Feature parsed
* FeatureCollection metadata encountered (bbox, id)
* Root metadata (bbox, id)

Derived classes may override any of these callbacks to implement:

* validation
* spatial indexing
* streaming transformations
* feature filtering
* custom data extraction

Doxygen API Reference
---------------------

.. doxygenclass:: GeoJSON::IO::Feature_Parser
   :project: GeoJSON
   :members:
   :protected-members:
   :private-members:
   :undoc-members:

Usage Example
-------------

Below is a minimal example of how to implement a custom parser
using the CRTP mechanism:

.. code-block:: cpp

    #include <io/feature_parser.h>

    class MyParser : public GeoJSON::IO::Feature_Parser<MyParser>
    {
    public:
        bool On_Full_Feature(::GeoJSON::Feature&& feature)
        {
            // Handle feature
            return true;
        }

        bool On_Geometry(::GeoJSON::Geometry&& geometry, std::size_t index)
        {
            // Handle geometry
            return true;
        }
    };

Integration in Documentation Tree
---------------------------------

Add this page to your IO / Parsing section:

.. toctree::
    :maxdepth: 1

    feature_parser
