pygeoflow.geojson
=================

This module exposes core GeoJSON data structures to Python.
It provides lightweight, mutable representations of GeoJSON objects
and is intended to be used together with ``pygeoflow.io`` and
``pygeoflow.dcel``.

The bindings closely mirror the underlying C++ data model and do not
perform validation beyond basic type consistency.

Module Overview
---------------

The ``pygeoflow.geojson`` module exposes the following categories:

- Spatial primitives (Position, Bbox)
- Properties
- Geometry types (Point, LineString, Polygon, …)
- Feature and FeatureCollection containers
- Root container for parsed GeoJSON documents


Position
--------

Represents a geographic coordinate.

**Class:** ``Position``

Constructors
^^^^^^^^^^^^

- ``Position()``
- ``Position(longitude: float, latitude: float)``
- ``Position(longitude: float, latitude: float, altitude: Optional[float])``

Attributes
^^^^^^^^^^

- ``longitude`` (float)
- ``latitude`` (float)
- ``altitude`` (Optional[float])

Representation
^^^^^^^^^^^^^^

``repr(Position)`` returns a human-readable string showing coordinates.


Bbox
----

Represents a GeoJSON bounding box.

**Class:** ``Bbox``

Methods
^^^^^^^

- ``Has_Altitude() -> bool``  Returns True if the bounding box includes altitude values.

- ``Get() -> tuple``  Returns the bounding box as a tuple without altitude.

- ``Get_With_Altitudes() -> tuple``  Returns the bounding box including altitude values, if present.


Property
--------

Represents a GeoJSON property value.

**Class:** ``Property``

Supported Types
^^^^^^^^^^^^^^^

- null
- bool
- int
- int64
- double
- string
- array
- object

Constructors
^^^^^^^^^^^^

- ``Property()``
- ``Property(bool)``
- ``Property(int)``
- ``Property(int64)``
- ``Property(double)``
- ``Property(str)``

Methods
^^^^^^^

- ``Get() -> Any``  
  Returns the property value as a native Python object.

- ``Set(value: Any) -> None``  
  Assigns a Python value to the property.

Representation
^^^^^^^^^^^^^^

``repr(Property)`` describes the stored type and value.


Point
-----

A GeoJSON Point geometry.

**Class:** ``Point``

Attributes
^^^^^^^^^^

- ``position`` (Position)


LineString
----------

A GeoJSON LineString geometry.

**Class:** ``LineString``

Attributes
^^^^^^^^^^

- ``positions`` (List[Position])


Polygon
-------

A GeoJSON Polygon geometry.

**Class:** ``Polygon``

Attributes
^^^^^^^^^^

- ``rings`` (List[List[Position]])


MultiPoint
----------

A GeoJSON MultiPoint geometry.

**Class:** ``MultiPoint``

Attributes
^^^^^^^^^^

- ``points`` (List[Point])


MultiLineString
---------------

A GeoJSON MultiLineString geometry.

**Class:** ``MultiLineString``

Attributes
^^^^^^^^^^

- ``line_strings`` (List[LineString])


MultiPolygon
------------

A GeoJSON MultiPolygon geometry.

**Class:** ``MultiPolygon``

Attributes
^^^^^^^^^^

- ``polygons`` (List[Polygon])


GeometryCollection
------------------

A collection of heterogeneous geometries.

**Class:** ``GeometryCollection``

Attributes
^^^^^^^^^^

- ``geometries`` (List[Geometry])


Geometry
--------

A tagged union representing any GeoJSON geometry type.

**Class:** ``Geometry``

Attributes
^^^^^^^^^^

- ``bbox`` (Optional[Bbox])

Type Inspection
^^^^^^^^^^^^^^^

- ``Is_Point() -> bool``
- ``Is_Multi_Point() -> bool``
- ``Is_Line_String() -> bool``
- ``Is_Multi_Line_String() -> bool``
- ``Is_Polygon() -> bool``
- ``Is_Multi_Polygon() -> bool``
- ``Is_Geometry_Collection() -> bool``

Accessors
^^^^^^^^^

- ``Get_Point() -> Point``
- ``Get_Multi_Point() -> MultiPoint``
- ``Get_Line_String() -> LineString``
- ``Get_Multi_Line_String() -> MultiLineString``
- ``Get_Polygon() -> Polygon``
- ``Get_Multi_Polygon() -> MultiPolygon``
- ``Get_Geometry_Collection() -> GeometryCollection``


Feature
-------

A GeoJSON Feature object.

**Class:** ``Feature``

Attributes
^^^^^^^^^^

- ``geometry`` (Optional[Geometry])
- ``properties`` (Property)
- ``id`` (Optional[str])
- ``bbox`` (Optional[Bbox])


FeatureCollection
-----------------

A GeoJSON FeatureCollection.

**Class:** ``FeatureCollection``

Attributes
^^^^^^^^^^

- ``features`` (List[Feature])
- ``bbox`` (Optional[Bbox])
- ``id`` (Optional[str])


Root
----

Top-level container for parsed GeoJSON documents.

**Class:** ``Root``

Type Inspection
^^^^^^^^^^^^^^^

- ``Is_Feature() -> bool``
- ``Is_Feature_Collection() -> bool``
- ``Is_Geometry() -> bool``

Accessors
^^^^^^^^^

- ``Get_Feature() -> Feature``
- ``Get_Feature_Collection() -> FeatureCollection``
- ``Get_Geometry() -> Geometry``


Typical Usage
-------------

GeoJSON data is usually loaded via ``pygeoflow.io`` which returns a ``Root`` object. The ``Root`` object can then be inspected to determine
its concrete type and passed to other modules such as ``pygeoflow.dcel`` for further processing.

This module does not perform I/O by itself.
