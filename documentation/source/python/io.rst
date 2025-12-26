pygeoflow.io
============

This module provides GeoJSON input and parsing facilities.
It exposes multiple parser interfaces built on top of a RapidJSON SAX
parser, allowing different parsing strategies depending on the use case.

The ``pygeoflow.io`` module is responsible for:

- Reading GeoJSON from files or strings
- Validating GeoJSON structure and geometry constraints
- Reporting detailed parsing and semantic errors
- Producing either a full in-memory GeoJSON object or streaming callbacks

This module does **not** perform any geometry processing by itself.
It is typically used together with ``pygeoflow.geojson`` and
``pygeoflow.dcel``.


Error
-----

Enumeration describing all possible parsing and validation errors.

**Enum:** ``Error``

Values
^^^^^^

- ``NO_ERROR``  Parsing completed successfully.
- ``FILE_OPENNING_FAILED``  The input file could not be opened.
- ``PARSING_ERROR``  Generic JSON parsing error.
- ``UNKNOWN_ROOT_OBJECT``  Root object is not a valid GeoJSON type.
- ``UNKNOWN_TYPE``  Unknown or unsupported GeoJSON type encountered.
- ``UNKNOWN_GEOMETRY_TYPE``  Geometry type string is invalid.
- ``COORDINATE_UNDERSIZED``  Coordinate array has too few elements.
- ``COORDINATE_OVERSIZED``  Coordinate array has too many elements.
- ``INCONSCISTENT_COORDINATE_LEVEL``  Coordinate nesting level is inconsistent.
- ``BAD_COORDINATE_FOR_GEMETRY``  Coordinate structure does not match geometry type.
- ``NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING``  LineString must contain at least two positions.
- ``NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON``  Polygon ring must contain at least four positions.
- ``POLYGON_NEED_TO_BE_CLOSED``  Polygon ring is not closed.
- ``POLYGON_NEED_AT_LEAST_ONE_RING``  Polygon must contain at least one ring.
- ``BBOX_SIZE_INCONSISTENT``  Bounding box size is invalid.
- ``PROPERTY_KEY_ALREADY_EXIST``  Duplicate property key detected.
- ``UNEXPECTED_STATE_KEY``  Unexpected JSON key encountered.
- ``UNEXPECTED_STATE_VALUE``  Unexpected JSON value encountered.
- ``UNEXPECTED_STATE_OBJECT``  Unexpected JSON object encountered.
- ``UNEXPECTED_STATE_ARRAY``  Unexpected JSON array encountered.
- ``UNEXPECTED_PROPERTY_STATE``  Invalid property state encountered.
- ``RECURSIVE_GEMETRY_COLLECTION_UNSUPPORTED``  Recursive GeometryCollection is not supported.
- ``GEOMETRY_COLLECTION_ELLEMENT_COUNT_MISMATCH``  GeometryCollection element count mismatch.

All enum values are exported directly into the module namespace.


FullParser
----------

Parses a complete GeoJSON document and produces a fully materialized
``pygeoflow.geojson.Root`` object.

**Class:** ``FullParser``

This parser is suitable when the entire GeoJSON document must be
loaded into memory.

Constructors
^^^^^^^^^^^^

- ``FullParser()``

Methods
^^^^^^^

- ``Parse_From_File(path: Path) -> Error``  Parses a GeoJSON document from a file.
- ``Parse_From_String(data: str) -> Error``  Parses a GeoJSON document from a string.
- ``Get_Geojson() -> pygeoflow.geojson.Root``  Returns the parsed GeoJSON root object.

Notes
^^^^^

If parsing fails, ``Get_Geojson()`` returns an undefined value.
The return code of the parse methods must always be checked.


Feature_Parser
--------------

Base class for feature-based streaming parsing.

**Class:** ``Feature_Parser``

This class is intended to be subclassed in Python.
It allows reacting to GeoJSON features as they are parsed,
without storing the entire document in memory.

Constructors
^^^^^^^^^^^^

- ``Feature_Parser()``

Methods
^^^^^^^

- ``Parse_From_File(path: Path) -> Error``  Parses a GeoJSON document from a file.
- ``Parse_From_String(data: str) -> Error``  Parses a GeoJSON document from a string.

Usage
^^^^^

Users typically override callback methods (exposed through C++ inheritance)
to process features incrementally.

This parser is commonly used internally by the DCEL builder.


Sax_Parser
----------

Low-level SAX-style parser interface.

**Class:** ``Sax_Parser``

This parser exposes a minimal streaming interface and is intended for
advanced use cases requiring full control over parsing events.

Constructors
^^^^^^^^^^^^

- ``Sax_Parser()``

Methods
^^^^^^^

- ``Parse_From_File(path: Path) -> Error``  Parses a GeoJSON document from a file.
- ``Parse_From_String(data: str) -> Error``  Parses a GeoJSON document from a string.

Notes
^^^^^

This parser does not build any GeoJSON object by default. It is intended to be subclassed to handle SAX events.


Path
----

Filesystem path wrapper.

**Class:** ``Path``

Constructors
^^^^^^^^^^^^

- ``Path(path: str)``

Implicit Conversions
^^^^^^^^^^^^^^^^^^^^

Python strings are implicitly convertible to ``Path``.
All file-based parse methods accept either ``Path`` or ``str``.


Typical Usage
-------------

Load a complete GeoJSON document:

::

    from pygeoflow.io import FullParser, Error

    parser = FullParser()
    ret = parser.Parse_From_File("data.geojson")

    if ret != Error.NO_ERROR:
        raise RuntimeError(ret)

    root = parser.Get_Geojson()

Streaming parse for DCEL construction:

::

    from pygeoflow.dcel import DCEL_Builder

    builder = DCEL_Builder(config)
    ret = builder.Parse_GeoJSON_File("data.geojson")


Design Notes
------------

- Parsing is strict and validates GeoJSON semantics.
- Errors are explicit and must be handled by the caller.
- File and string parsing share the same internal logic.
- Streaming parsers are preferred for large datasets.