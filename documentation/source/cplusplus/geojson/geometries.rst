.. _geojson_geometry:

GeoJSON Geometry Types
======================

.. contents::
   :local:
   :depth: 2

Overview
--------

The **GeoJSON geometry model** defines spatial objects as simple, nested data structures. Each geometry type corresponds to one or more sets of coordinates, following the GeoJSON specification (RFC 7946).

Each type is represented by a C++ struct inside the ``GeoJSON`` namespace.

.. note::

   All examples and structures are compliant with the `GeoJSON Specification <https://datatracker.ietf.org/doc/html/rfc7946>`_.

Position
--------

.. doxygenstruct:: GeoJSON::Position
   :project: GeoJSON
   :members:
   :undoc-members:
   :protected-members:
   :private-members:

Point
-----

.. doxygenstruct:: GeoJSON::Point
   :project: GeoJSON
   :members:

LineString
-----------

.. doxygenstruct:: GeoJSON::Line_String
   :project: GeoJSON
   :members:

MultiPoint
-----------

.. doxygenstruct:: GeoJSON::Multi_Point
   :project: GeoJSON
   :members:

MultiLineString
---------------

.. doxygenstruct:: GeoJSON::Multi_Line_String
   :project: GeoJSON
   :members:

Polygon
--------

.. doxygenstruct:: GeoJSON::Polygon
   :project: GeoJSON
   :members:

MultiPolygon
------------

.. doxygenstruct:: GeoJSON::Multi_Polygon
   :project: GeoJSON
   :members:

GeometryCollection
------------------

.. doxygenstruct:: GeoJSON::Geometry_Collection
   :project: GeoJSON
   :members: