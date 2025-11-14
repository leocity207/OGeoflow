.. _geojson_decorations:

GeoJSON Decorations
===================

The **GeoJSON Decorations** represent supplementary information that can be attached to any GeoJSON object — such as *properties*, *bounding boxes*, or *identifiers*. These structures provide additional descriptive or spatial context without altering the core geometric data.

.. contents::
   :local:
   :depth: 2

---

Properties
----------

.. doxygenstruct:: GeoJSON::Property
   :project: geojson
   :members:
   :protected-members:
   :undoc-members:
   :brief:

The :cpp:struct:`GeoJSON::Property` structure models arbitrary JSON-like key/value data. It is used primarily in :cpp:struct:`GeoJSON::Feature` to store non-spatial attributes such as names, categories, or numerical metadata.

**Example Usage:**

.. code-block:: cpp

   GeoJSON::Property prop;
   prop = GeoJSON::Property::Object{
       {"name", "Central Park"},
       {"area", 3.41},
       {"tags", GeoJSON::Property::Array{"park", "public"}}
   };

   if (prop.Is_Object()) {
       const auto& obj = prop.Get_Object();
       std::cout << obj.at("name").Get_String(); // Central Park
   }

---

Bounding Boxes
---------------

.. doxygenstruct:: GeoJSON::Bbox
   :project: geojson
   :members:
   :protected-members:
   :undoc-members:
   :brief:

A :cpp:struct:`GeoJSON::Bbox` represents the spatial extent of an object. It can contain either 2D (`minX, minY, maxX, maxY`) or 3D (`minX, minY, minZ, maxX, maxY, maxZ`) values depending on the geometry.

Bounding boxes are optional and used primarily for quick spatial filtering, rendering optimizations, or summaries of feature collections.

---

Identifiers
------------

In GeoJSON, **identifiers** are optional metadata fields used to uniquely identify a feature or collection. In this implementation, identifiers are represented simply as:

.. code-block:: cpp

   std::optional<std::string> id;

The identifier may hold any unique string (UUID, numeric string, etc.). When absent, the object is considered *anonymous*.

Identifiers are typically found in:
 - :cpp:struct:`GeoJSON::Feature`
 - :cpp:struct:`GeoJSON::Feature_Collection`

---

Summary
--------

+--------------------+----------------------------------------------+
| **Decoration**     | **Purpose**                                  |
+====================+==============================================+
| Property           | Arbitrary metadata and descriptive fields.   |
+--------------------+----------------------------------------------+
| Bbox               | Defines spatial extent (2D/3D).              |
+--------------------+----------------------------------------------+
| ID                 | Optional unique identifier.                  |
+--------------------+----------------------------------------------+

These decorations form the descriptive and structural layer of GeoJSON objects, enriching spatial data with attributes and spatial boundaries while maintaining lightweight interoperability.