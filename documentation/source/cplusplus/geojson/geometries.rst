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

.. doxygenstruct:: O::GeoJSON::Position
   :members:
   :undoc-members:
   :protected-members:
   :private-members:

Point
-----

.. figure:: ../../../image/SFA_Point.svg

   Point in GeoJSON (Mwtoews, CC BY-SA 3.0)

.. doxygenstruct:: O::GeoJSON::Point
   :members:

LineString
-----------

.. figure:: ../../../image/SFA_LineString.svg

   LineString in GeoJSON (Mwtoews, CC BY-SA 3.0)

.. doxygenstruct:: O::GeoJSON::Line_String
   :members:

MultiPoint
-----------

.. figure:: ../../../image/SFA_MultiPoint.svg

   Multipoint in GeoJSON (Mwtoews, CC BY-SA 3.0)

.. doxygenstruct:: O::GeoJSON::Multi_Point
   :members:

MultiLineString
---------------

.. figure:: ../../../image/SFA_MultiLineString.svg

   MultiLinestring in GeoJSON (Mwtoews, CC BY-SA 3.0)

.. doxygenstruct:: O::GeoJSON::Multi_Line_String
   :members:

Polygon
--------

.. figure:: ../../../image/SFA_Polygon.svg

   Polygon with one ring in GeoJSON (Mwtoews, CC BY-SA 3.0)

.. figure:: ../../../image/SFA_Polygon_with_hole.svg

   Polygon with two ring creating holes in GeoJSON (Mwtoews, CC BY-SA 3.0)

.. doxygenstruct:: O::GeoJSON::Polygon
   :members:

MultiPolygon
------------

.. figure:: ../../../image/SFA_MultiPolygon.svg

   Polygons with one ring each in GeoJSON (Mwtoews, CC BY-SA 3.0)

.. figure:: ../../../image/SFA_MultiPolygon_with_hole.svg

   Polygons with multiple rings creating holes in GeoJSON (Mwtoews, CC BY-SA 3.0)

.. doxygenstruct:: O::GeoJSON::Multi_Polygon
   :members:

GeometryCollection
------------------

.. doxygenstruct:: O::GeoJSON::Geometry_Collection
   :members: