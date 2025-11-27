.. _geojson_root:

GeoJSON Root Object
===================

The **GeoJSON** structure is the unified top-level representation of any valid GeoJSON entity — including geometries, features, and feature collections.

It provides a consistent C++ abstraction for working with data that conforms to the GeoJSON specification (RFC 7946).

.. contents::
   :local:
   :depth: 2

---

GeoJSON Root
-----------------

.. doxygenstruct:: O::GeoJSON::Root
   :members:
   :protected-members:
   :undoc-members:

The :cpp:struct:`O::GeoJSON::Root` type acts as a *variant container* that can hold any of the three primary GeoJSON object types:

- :cpp:struct:`O::GeoJSON::Geometry`
- :cpp:struct:`O::GeoJSON::Feature`
- :cpp:struct:`O::GeoJSON::Feature_Collection`

It exposes type introspection helpers (`Is_*`), along with safe getters (`Get_*`) to access the underlying data.

**Example:**

.. code-block:: cpp

   GeoJSON::Root root;
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

.. doxygenenum:: O::GeoJSON::Type

**Object Keys**

.. doxygenenum:: O::GeoJSON::Key

These enums are useful for constructing or interpreting GeoJSON documents programmatically without relying on raw string comparisons.

---

String Conversion Utilities
----------------------------

.. doxygenfunction:: O::GeoJSON::String_To_Type

.. doxygenfunction:: O::GeoJSON::String_To_Key

These inline functions translate string-based JSON members into the corresponding strongly typed enums for faster and safer parsing logic.