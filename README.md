![Octoliner GeoFlow Logo](logo.svg)

# Octoliner GeoFlow

**A small, library that can be used to parse/write GeoJSON file into an easy to use structure or transformed into a Doubly Connected Edge list.**

Octoliner GeoFlow provides a simple, type-safe way to serialize and
deserialize GeoJSON using rapidjson for fast and simple parsing. It is intentionally
minimal and designed to be embedded into larger projects.

This library is part of the larger **Octoliner** family of tools (hence the `O` prefix).

---

## Highlights

- modern C++ 20 (templates + CRTP).
- Cmpliance with the RFC 7946 GeoJSON
- GeoJSON Serialization/Deserialization.
- Doubly Connected Edge list <-> GeoJSON structure.
- simple holdind structure.
- Designed to be easy to fetch & embed via CMake `FetchContent`.

---

## Quick install (CMake + FetchContent)

Drop this into your top-level `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
    OGeoJSON
    GIT_REPOSITORY https://github.com/leocity207/OGeoJSON.git
    UPDATE_DISCONNECTED ON
)

FetchContent_MakeAvailable(OGeoJSON)
```

## Quick Exemple

easy GeoJSON Parsing

```C++

#include <io/feature_parser.h>

class MyParser : public O::GeoJSON::IO::Feature_Parser<MyParser>
{
public:
    bool On_Full_Feature(O::GeoJSON::Feature&& feature)
    {
        // Handle feature
        return true;
    }

    bool On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t index)
    {
        // Handle geometry
        return true;
    }
};

int main()
{
    rapidjson::Reader reader;
    MyParser handler;
    
    rapidjson::StringStream ss("A valid JSON string");
    
    if (!reader.Parse(ss, handler) )
    {
        // handle parsing error
    }
    if (auto geojson = handler.Get_Geojson())
    {
        //get the geoJSON
    }
}
```

DCEL Creation

```C++
#include <io/feature_Parser.h>
#include <dcel/builder.h>

class Auto_Builder : public O::DCEL::Builder, public O::GeoJSON::IO::Feature_Parser<O::DCEL::Builder> {};

int main()
{
    Auto_Builder auto_builder;
    rapidjson::StringStream ss("Some JSON value");
    bool ok = reader.Parse(ss, auto_builder);

    auto opt_dcel = auto_builder.Get_Dcel();
    auto opt_feature_infr = auto_builder.Get_Feature_Info(); 
}
```