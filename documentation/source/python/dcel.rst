pygeoflow.dcel
==============

The ``pygeoflow.dcel`` module provides tools to construct and inspect a
**DCEL (Doubly Connected Edge List)** from GeoJSON data.

It exposes builders that transform GeoJSON into a DCEL storage, as well as
Python-accessible views of vertices, half-edges, faces, and configuration
objects.

The module is designed for **topological inspection**, **graph traversal**,
and **geometry analysis**, not for mutation.


Overview
--------

Typical workflow:

1. Configure DCEL construction parameters
2. Parse a GeoJSON document (file or string)
3. Retrieve the resulting DCEL storage
4. Traverse vertices, half-edges, and faces


DCEL Builders
-------------

DCEL_Builder
~~~~~~~~~~~~

.. class:: DCEL_Builder(config)

	Builds a DCEL from GeoJSON input without filtering.

	:param config: DCEL configuration parameters
	:type config: Configuration

	.. method:: Parse(geojson)

		Parse a GeoJSON root object.

		:param geojson: A GeoJSON root value
		:type geojson: pygeoflow.geojson.Root
		:return: Parsing status
		:rtype: pygeoflow.io.Error

	.. method:: Parse_GeoJSON_File(path)

		Parse a GeoJSON file from disk.

		:param path: Path to a GeoJSON file
		:type path: str or pygeoflow.io.Path
		:return: Parsing status
		:rtype: pygeoflow.io.Error

	.. method:: Parse_GeoJSON_String(string)

		Parse a GeoJSON document from a string.

		:param string: GeoJSON text
		:type string: str
		:return: Parsing status
		:rtype: pygeoflow.io.Error

	.. method:: DCEL()

		Access the constructed DCEL storage.

		:return: DCEL storage
		:rtype: Storage

	.. method:: Feature_Info()

		Access feature-to-DCEL mapping information.

		:return: Feature metadata
		:rtype: Feature_Info


DCEL_Filtered_Builder
~~~~~~~~~~~~~~~~~~~~~

.. class:: DCEL_Filtered_Builder(config, predicate)

	Builds a DCEL from GeoJSON input while filtering features using a
	Python predicate.

	:param config: DCEL configuration parameters
	:type config: Configuration
	:param predicate: Callable taking a GeoJSON Feature and returning bool
	:type predicate: Callable[[pygeoflow.geojson.Feature], bool]

	.. method:: Parse_GeoJSON_File(path)

	.. method:: Parse_GeoJSON_String(string)

	.. method:: DCEL()

	.. method:: Feature_Info()

	These methods behave identically to those in :class:`DCEL_Builder`,
	but only features for which the predicate returns ``True`` are included.


DCEL Storage
------------

Storage
~~~~~~~

.. class:: Storage

	Container holding all DCEL elements.

	The storage owns all vertices, half-edges, and faces.

	.. attribute:: vertices

		List of all vertices.

		:type: list[Vertex]

	.. attribute:: half_edges

		List of all half-edges.

		:type: list[HalfEdge]

	.. attribute:: faces

		List of all faces.

		:type: list[Face]


DCEL Elements
------------

Vertex
~~~~~~

.. class:: Vertex

	A vertex in the DCEL.

	Vertices are immutable from Python and provide access to their
	outgoing half-edges.

	.. attribute:: x

		X coordinate.

		:type: float

	.. attribute:: y

		Y coordinate.

		:type: float

	.. attribute:: outgoing_edges

		Outgoing half-edges originating from this vertex.

		Each half-edge is returned by value.

		:type: list[HalfEdge]


HalfEdge
~~~~~~~~

.. class:: HalfEdge

	Directed edge in the DCEL.

	Each half-edge has a twin with opposite direction and participates
	in a circular linked list around a face.

	.. attribute:: tail

		Origin vertex of the half-edge.

		:type: Vertex

	.. attribute:: head

		Destination vertex of the half-edge.

		:type: Vertex

	.. attribute:: twin

		Opposite half-edge.

		:type: HalfEdge

	.. attribute:: next

		Next half-edge around the face.

		:type: HalfEdge

	.. attribute:: prev

		Previous half-edge around the face.

		:type: HalfEdge

	.. attribute:: face

		Face to which this half-edge belongs.

		:type: Face


Face
~~~~

.. class:: Face

	A face in the DCEL.

	Faces are bounded by a cycle of half-edges.

	.. attribute:: edge

		One half-edge on the boundary of the face.

		:type: HalfEdge


Configuration
-------------

Merge_Strategy
~~~~~~~~~~~~~~

.. enum:: Merge_Strategy

	Strategy used to merge vertices during construction.

	.. value:: AT_FIRST

		Merge at first matching vertex.


Configuration
~~~~~~~~~~~~~

.. class:: Configuration

	Configuration parameters controlling DCEL construction.

	.. attribute:: max_vertices

		Maximum number of vertices.

		:type: int

	.. attribute:: max_half_edges

		Maximum number of half-edges.

		:type: int

	.. attribute:: max_faces

		Maximum number of faces.

		:type: int

	.. attribute:: position_tolerance

		Distance tolerance for vertex merging.

		:type: float

	.. attribute:: vertex_merge_strategy

		Strategy used to merge vertices.

		:type: Merge_Strategy


Notes
-----

* All DCEL elements are **read-only** from Python.
* Object identity is **not preserved** across property access; equalityshould not rely on object identity.
* Half-edge topology (next/prev/twin/face) is guaranteed to be consistent for valid input GeoJSON.