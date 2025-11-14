.. _geojson_objects:

GeoJSON Object Types
====================

.. contents::
	:local:
	:depth: 2

Overview
--------

The GeoJSON *object layer* defines how spatial data is represented as structured
JSON documents. Three main object types are provided:

1. **Geometry** – describes the shape of spatial entities.
2. **Feature** – combines geometry with properties (attributes).
3. **FeatureCollection** – groups multiple features together.

Each of these C++ structures corresponds to a GeoJSON specification concept
as defined in RFC 7946.

.. note::

	All examples and schemas are compliant with the
	`GeoJSON Specification <https://datatracker.ietf.org/doc/html/rfc7946>`_.

Geometry
--------

.. doxygenstruct:: GeoJSON::Geometry
	:project: GeoJSON
	:members:
	:protected-members:
	:undoc-members:

Feature
-------

.. doxygenstruct:: GeoJSON::Feature
	:project: GeoJSON
	:members:
	:protected-members:
	:undoc-members:

FeatureCollection
-----------------

.. doxygenstruct:: GeoJSON::Feature_Collection
	:project: GeoJSON
	:members:
	:protected-members:
	:undoc-members: