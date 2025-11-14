.. _geojson_root:

GeoJSON Root Object
===================

The **GeoJSON** structure is the unified top-level representation of any valid GeoJSON entity — including geometries, features, and feature collections.

It provides a consistent C++ abstraction for working with data that conforms to the GeoJSON specification (RFC 7946).

.. contents::
   :local:
   :depth: 2

---

GeoJSON Container
-----------------

.. doxygenstruct:: GeoJSON::GeoJSON
   :project: geojson
   :members:
   :protected-members:
   :undoc-members:
   :brief:

The :cpp:struct:`GeoJSON::GeoJSON` type acts as a *variant container* that can hold any of the three primary GeoJSON object types:

- :cpp:struct:`GeoJSON::Geometry`
- :cpp:struct:`GeoJSON::Feature`
- :cpp:struct:`GeoJSON::Feature_Collection`

It exposes type introspection helpers (`Is_*`), along with safe getters (`Get_*`) to access the underlying data.

**Example:**

.. code-block:: cpp

   GeoJSON::GeoJSON root;
   root.object = GeoJSON::Feature_Collection{};

   if (root.Is_Feature_Collection()) {
       auto& collection = root.Get_Feature_Collection();
       std::cout << "Feature count: " << collection.features.size() << std::endl;
   }

---

Enumerations
------------

The GeoJSON implementation defines two helper enums to categorize object types and keys found in serialized documents.

**Object Types**

.. doxygenenum:: GeoJSON::Type
   :project: geojson:

**Object Keys**

.. doxygenenum:: GeoJSON::Key
   :project: geojson:

These enums are useful for constructing or interpreting GeoJSON documents programmatically without relying on raw string comparisons.

---

String Conversion Utilities
----------------------------

.. doxygenfunction:: GeoJSON::String_To_Type
   :project: geojson:

.. doxygenfunction:: GeoJSON::String_To_Key
   :project: geojson:

These inline functions translate string-based JSON members into the corresponding strongly typed enums for faster and safer parsing logic.