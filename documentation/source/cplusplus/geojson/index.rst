.. _geojson:

GeoJSON
=======

The **O::GeoJSON** namespace defines all data structures that describe the content of a GeoJSON file.

This section documents:

* The **three main GeoJSON object types**:
  - :cpp:struct:`O::GeoJSON::Geometry`
  - :cpp:struct:`O::GeoJSON::Feature`
  - :cpp:struct:`O::GeoJSON::Feature_Collection`
* The different **geometry types** supported by GeoJSON (Point, LineString, Polygon, etc.)
* The **decorations** used to enrich GeoJSON objects, such as:
  - :cpp:struct:`O::GeoJSON::Bbox`
  - :cpp:type:`std::optional<std::string>` as an **ID**

.. toctree::
	:maxdepth: 2
	:caption: Contents

	geojson
	geometries
	object
	decoration