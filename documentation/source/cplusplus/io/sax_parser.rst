.. _sax_parser:

GeoJSON::IO::Sax_Parser
=======================

The :cpp:class:`GeoJSON::IO::SAX_Parser` class implements a streaming,
state-driven parser for GeoJSON using RapidJSON’s SAX interface.

It is designed for **high-performance, low-memory** parsing of large
GeoJSON files. Instead of building an entire document tree, the parser
streams objects as they are completed and notifies the user through three
callback functions.

Purpose
-------

The parser reconstructs GeoJSON structures incrementally while keeping an
internal stack that reflects the reader’s position inside the JSON
document. Depending on the current state (Feature, Geometry, Coordinates,
Properties, BBox…), different behaviors are applied.

To use the parser, derive from :cpp:class:`GeoJSON::IO::SAX_Parser` and
implement the following three required callback functions:

Required Callback Functions
---------------------------

.. cpp:function:: bool On_Geometry(GeoJSON::Geometry&& geometry, std::size_t element_number)

   Called whenever a complete :cpp:struct:`GeoJSON::Geometry` has been parsed.
   The geometry may be a top-level Geometry or an element of a GeometryCollection.
   
   :param geometry: Fully constructed GeoJSON geometry.
   :param element_number: Index if inside a geometry collection.
   :returns: ``true`` to continue parsing, ``false`` to abort.

.. cpp:function:: bool On_Feature(GeoJSON::Feature&& feature)

   Called when a :cpp:struct:`GeoJSON::Feature` object has been completely parsed.
   Geometry are always empty.

   :param feature: Parsed GeoJSON feature.
   :returns: ``true`` to continue parsing, ``false`` to abort.

.. cpp:function:: bool On_Feature_Collection(std::optional<Bbox>&& bbox, std::optional<std::string>&& id)

   Called after all features inside a FeatureCollection have been streamed. Features are always empty
   through :cpp:func:`On_Feature`.

   :param bbox: Optional bounding box for the collection.
   :param id: Optional collection identifier.
   :returns: ``true`` to continue parsing, ``false`` to abort.

Internal State Machine
----------------------

The parser uses an internal enum :cpp:enum:`GeoJSON::IO::SAX_Parser::Parse_State`
to drive behavior depending on what part of the GeoJSON structure is being processed.

States include:

* ``ROOT``
* ``FEATURE``
* ``GEOMETRY``
* ``COORDINATES``
* ``BBOX``
* ``PROPERTIES``
* ...and many more.

DOT Workflow Diagrams
---------------------

The RapidJSON SAX interface triggers callbacks in a non-trivial order.
To assist developers, the parsing workflow for each handler is documented
using DOT diagrams:

.. figure:: _static/dot/startObject.png
   :align: center
   :alt: StartObject flow diagram

   **StartObject() workflow**

.. figure:: _static/dot/endObject.png
   :align: center
   :alt: EndObject flow diagram

   **EndObject() workflow**

.. figure:: _static/dot/startArray.png
   :align: center
   :alt: StartArray flow diagram

   **StartArray() workflow**

.. figure:: _static/dot/endArray.png
   :align: center
   :alt: EndArray flow diagram

   **EndArray() workflow**

.. figure:: _static/dot/key.png
   :align: center
   :alt: Key workflow

   **Key() workflow**

.. figure:: _static/dot/value.png
   :align: center
   :alt: Number/String/Bool workflow

   **Value parsing workflow**

These diagrams show how the parser transitions between states and how
control flows to different helper functions such as:

* ``Push_Context()``
* ``Pop_Context()``
* ``Set_Current_Key()``
* ``Process_Coordinate_Number()``
* ``Finalize_Coordinates()``


Usage Example
-------------

A user wanting to process GeoJSON streams must extend the parser:

.. code-block:: cpp

   class MyParser : public GeoJSON::IO::SAX_Parser<MyParser>
   {
   public:
       bool On_Geometry(GeoJSON::Geometry&& geom, std::size_t index) override
       {
           // Handle geometry
           return true;
       }

       bool On_Feature(GeoJSON::Feature&& feature) override
       {
           // Handle feature
           return true;
       }

       bool On_Feature_Collection(std::optional<Bbox>&& bbox,
                                  std::optional<std::string>&& id) override
       {
           // Finalize collection
           return true;
       }
   };

   int main()
   {
   	   rapidjson::StringStream ss("...");
       rapidjson::Reader reader;
       MyParser parser;
       bool ok = reader.Parse(ss, parser);
   }

implementation
--------------

.. doxygenstruct:: GeoJSON::IO::SAX_Parser
   :members:
   :protected-members:
   :undoc-members:
   :brief:

See Also
--------

* :ref:`geojson`
* :ref:`geometries`
* :ref:`object`
* :ref:`decoration`
