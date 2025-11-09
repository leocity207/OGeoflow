#ifndef PYBIND_IO_H
#define PYBIND_IO_H

#include <pybind11/pybind11.h>

void Init_Geojson_Bindings(py::module_ &m) {
    // Position
    py::class_<GeoJSON::Position>(m, "Position")
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<double, double, std::optional<double>>())
        .def_readwrite("longitude", &GeoJSON::Position::longitude)
        .def_readwrite("latitude", &GeoJSON::Position::latitude)
        .def_readwrite("altitude", &GeoJSON::Position::altitude)
        .def("__repr__", [](const GeoJSON::Position& p) {
            if (p.altitude) {
                return "Position(longitude=" + std::to_string(p.longitude) + 
                       ", latitude=" + std::to_string(p.latitude) + 
                       ", altitude=" + std::to_string(*p.altitude) + ")";
            }
            return "Position(longitude=" + std::to_string(p.longitude) + 
                   ", latitude=" + std::to_string(p.latitude) + ")";
        });
    
    // Bbox
    py::class_<GeoJSON::Bbox>(m, "Bbox")
        .def(py::init<>())
        .def("has_altitude", &GeoJSON::Bbox::Has_Altitude)
        .def("get", &GeoJSON::Bbox::Get)
        .def("get_with_altitudes", &GeoJSON::Bbox::Get_With_Altitudes)
        .def("__repr__", [](const GeoJSON::Bbox& b) {
            return "Bbox(has_altitude=" + std::to_string(b.Has_Altitude()) + ")";
        });
    
    // Property
    py::class_<GeoJSON::Property>(m, "Property")
        .def(py::init<>())
        .def(py::init<bool>())
        .def(py::init<int>())
        .def(py::init<int64_t>())
        .def(py::init<double>())
        .def(py::init<const char*>())
        .def(py::init<std::string>())
        .def("is_null", &GeoJSON::Property::Is_Null)
        .def("is_bool", &GeoJSON::Property::Is_Bool)
        .def("is_integer", &GeoJSON::Property::Is_Integer)
        .def("is_double", &GeoJSON::Property::Is_Double)
        .def("is_string", &GeoJSON::Property::Is_String)
        .def("is_array", &GeoJSON::Property::Is_Array)
        .def("is_object", &GeoJSON::Property::Is_Object)
        .def("get_bool", &GeoJSON::Property::Get_Bool)
        .def("get_int", &GeoJSON::Property::Get_Int)
        .def("get_double", &GeoJSON::Property::Get_Double)
        .def("get_string", &GeoJSON::Property::Get_String)
        .def("get_array", &GeoJSON::Property::Get_Array, py::return_value_policy::reference_internal)
        .def("get_object", &GeoJSON::Property::Get_Object, py::return_value_policy::reference_internal)
        .def("size", &GeoJSON::Property::Size)
        .def("clear", &GeoJSON::Property::Clear)
        .def("__repr__", [](const GeoJSON::Property& p) {
            if (p.Is_Null()) return "Property(null)";
            if (p.Is_Bool()) return "Property(bool=" + std::to_string(p.Get_Bool()) + ")";
            if (p.Is_Integer()) return "Property(int=" + std::to_string(p.Get_Int()) + ")";
            if (p.Is_Double()) return "Property(double=" + std::to_string(p.Get_Double()) + ")";
            if (p.Is_String()) return "Property(string='" + std::string(p.Get_String()) + "')";
            if (p.Is_Array()) return "Property(array[size=" + std::to_string(p.Size()) + "])";
            if (p.Is_Object()) return "Property(object[size=" + std::to_string(p.Size()) + "])";
            return "Property(unknown)";
        });
    
    // Point
    py::class_<GeoJSON::Point>(m, "Point")
        .def(py::init<>())
        .def_readwrite("position", &GeoJSON::Point::position)
        .def("__repr__", [](const GeoJSON::Point& p) {
            return "Point(" + std::to_string(p.position.longitude) + 
                   ", " + std::to_string(p.position.latitude) + ")";
        });
    
    // LineString
    py::class_<GeoJSON::Line_String>(m, "LineString")
        .def(py::init<>())
        .def_readwrite("positions", &GeoJSON::Line_String::positions)
        .def("__repr__", [](const GeoJSON::Line_String& ls) {
            return "LineString(positions=" + std::to_string(ls.positions.size()) + ")";
        });
    
    // Polygon
    py::class_<GeoJSON::Polygon>(m, "Polygon")
        .def(py::init<>())
        .def_readwrite("rings", &GeoJSON::Polygon::rings)
        .def("__repr__", [](const GeoJSON::Polygon& p) {
            return "Polygon(rings=" + std::to_string(p.rings.size()) + ")";
        });
    
    // MultiPoint
    py::class_<GeoJSON::Multi_Point>(m, "MultiPoint")
        .def(py::init<>())
        .def_readwrite("points", &GeoJSON::Multi_Point::points)
        .def("__repr__", [](const GeoJSON::Multi_Point& mp) {
            return "MultiPoint(points=" + std::to_string(mp.points.size()) + ")";
        });
    
    // MultiLineString
    py::class_<GeoJSON::Multi_Line_String>(m, "MultiLineString")
        .def(py::init<>())
        .def_readwrite("line_strings", &GeoJSON::Multi_Line_String::line_strings)
        .def("__repr__", [](const GeoJSON::Multi_Line_String& mls) {
            return "MultiLineString(line_strings=" + std::to_string(mls.line_strings.size()) + ")";
        });
    
    // MultiPolygon
    py::class_<GeoJSON::Multi_Polygon>(m, "MultiPolygon")
        .def(py::init<>())
        .def_readwrite("polygons", &GeoJSON::Multi_Polygon::polygons)
        .def("__repr__", [](const GeoJSON::Multi_Polygon& mp) {
            return "MultiPolygon(polygons=" + std::to_string(mp.polygons.size()) + ")";
        });
    
    // GeometryCollection
    py::class_<GeoJSON::Geometry_Collection>(m, "GeometryCollection")
        .def(py::init<>())
        .def_readwrite("geometries", &GeoJSON::Geometry_Collection::geometries)
        .def("__repr__", [](const GeoJSON::Geometry_Collection& gc) {
            return "GeometryCollection(geometries=" + std::to_string(gc.geometries.size()) + ")";
        });
    
    // Geometry Type enum
    py::enum_<GeoJSON::Geometry::Type>(m, "GeometryType")
        .value("POINT", GeoJSON::Geometry::Type::POINT)
        .value("MULTI_POINT", GeoJSON::Geometry::Type::MULTI_POINT)
        .value("LINE_STRING", GeoJSON::Geometry::Type::LINE_STRING)
        .value("MULTI_LINE_STRING", GeoJSON::Geometry::Type::MULTI_LINE_STRING)
        .value("POLYGON", GeoJSON::Geometry::Type::POLYGON)
        .value("MULTI_POLYGON", GeoJSON::Geometry::Type::MULTI_POLYGON)
        .value("GEOMETRY_COLLECTION", GeoJSON::Geometry::Type::GEOMETRY_COLLECTION)
        .value("UNKNOWN", GeoJSON::Geometry::Type::UNKNOWN)
        .export_values();
    
    // Geometry
    py::class_<GeoJSON::Geometry>(m, "Geometry")
        .def(py::init<>())
        .def_readwrite("value", &GeoJSON::Geometry::value)
        .def_readwrite("bbox", &GeoJSON::Geometry::bbox)
        .def("is_point", &GeoJSON::Geometry::Is_Point)
        .def("is_multi_point", &GeoJSON::Geometry::Is_Multi_Point)
        .def("is_line_string", &GeoJSON::Geometry::Is_Line_String)
        .def("is_multi_line_string", &GeoJSON::Geometry::Is_Multi_Line_String)
        .def("is_polygon", &GeoJSON::Geometry::Is_Polygon)
        .def("is_multi_polygon", &GeoJSON::Geometry::Is_Multi_Polygon)
        .def("is_geometry_collection", &GeoJSON::Geometry::Is_Geometry_Collection)
        .def("get_point", &GeoJSON::Geometry::Get_Point, py::return_value_policy::reference_internal)
        .def("get_multi_point", &GeoJSON::Geometry::Get_Multi_Point, py::return_value_policy::reference_internal)
        .def("get_line_string", &GeoJSON::Geometry::Get_Line_String, py::return_value_policy::reference_internal)
        .def("get_multi_line_string", &GeoJSON::Geometry::Get_Multi_Line_String, py::return_value_policy::reference_internal)
        .def("get_polygon", &GeoJSON::Geometry::Get_Polygon, py::return_value_policy::reference_internal)
        .def("get_multi_polygon", &GeoJSON::Geometry::Get_Multi_Polygon, py::return_value_policy::reference_internal)
        .def("get_geometry_collection", &GeoJSON::Geometry::Get_Geometry_Collection, py::return_value_policy::reference_internal)
        .def("string_to_type", &GeoJSON::Geometry::String_To_Type)
        .def("__repr__", [](const GeoJSON::Geometry& g) {
            if (g.Is_Point()) return "Geometry(Point)";
            if (g.Is_Multi_Point()) return "Geometry(MultiPoint)";
            if (g.Is_Line_String()) return "Geometry(LineString)";
            if (g.Is_Multi_Line_String()) return "Geometry(MultiLineString)";
            if (g.Is_Polygon()) return "Geometry(Polygon)";
            if (g.Is_Multi_Polygon()) return "Geometry(MultiPolygon)";
            if (g.Is_Geometry_Collection()) return "Geometry(GeometryCollection)";
            return "Geometry(Unknown)";
        });
    
    // Feature
    py::class_<GeoJSON::Feature>(m, "Feature")
        .def(py::init<>())
        .def_readwrite("geometry", &GeoJSON::Feature::geometry)
        .def_readwrite("properties", &GeoJSON::Feature::properties)
        .def_readwrite("id", &GeoJSON::Feature::id)
        .def_readwrite("bbox", &GeoJSON::Feature::bbox)
        .def("__repr__", [](const GeoJSON::Feature& f) {
            return "Feature(id=" + (f.id ? *f.id : "none") + 
                   ", geometry=" + (f.geometry ? "present" : "none") + ")";
        });
    
    // FeatureCollection
    py::class_<GeoJSON::Feature_Collection>(m, "FeatureCollection")
        .def(py::init<>())
        .def_readwrite("features", &GeoJSON::Feature_Collection::features)
        .def_readwrite("bbox", &GeoJSON::Feature_Collection::bbox)
        .def_readwrite("id", &GeoJSON::Feature_Collection::id)
        .def("__repr__", [](const GeoJSON::Feature_Collection& fc) {
            return "FeatureCollection(features=" + std::to_string(fc.features.size()) + ")";
        });
    
    // GeoJSON (main container)
    py::class_<GeoJSON::GeoJSON>(m, "GeoJSON")
        .def(py::init<>())
        .def_readwrite("object", &GeoJSON::GeoJSON::object)
        .def("is_feature", &GeoJSON::GeoJSON::Is_Feature)
        .def("is_feature_collection", &GeoJSON::GeoJSON::Is_Feature_Collection)
        .def("is_geometry", &GeoJSON::GeoJSON::Is_Geometry)
        .def("get_feature", &GeoJSON::GeoJSON::Get_Feature, py::return_value_policy::reference_internal)
        .def("get_feature_collection", &GeoJSON::GeoJSON::Get_Feature_Collection, py::return_value_policy::reference_internal)
        .def("get_geometry", &GeoJSON::GeoJSON::Get_Geometry, py::return_value_policy::reference_internal)
        .def("__repr__", [](const GeoJSON::GeoJSON& g) {
            if (g.Is_Feature()) return "GeoJSON(Feature)";
            if (g.Is_Feature_Collection()) return "GeoJSON(FeatureCollection)";
            if (g.Is_Geometry()) return "GeoJSON(Geometry)";
            return "GeoJSON(Unknown)";
        });
    
    // Enums
    py::enum_<GeoJSON::Key>(m, "Key")
        .value("TYPE", GeoJSON::Key::TYPE)
        .value("COORDINATES", GeoJSON::Key::COORDINATES)
        .value("FEATURES_COLLECTION", GeoJSON::Key::FEATURES_COLLECTION)
        .value("PROPERTIES", GeoJSON::Key::PROPERTIES)
        .value("GEOMETRY_COLLECTION", GeoJSON::Key::GEOMETRY_COLLECTION)
        .value("GEOMETRY", GeoJSON::Key::GEOMETRY)
        .value("ID", GeoJSON::Key::ID)
        .value("BBOX", GeoJSON::Key::BBOX)
        .value("FOREIGN", GeoJSON::Key::FOREIGN)
        .export_values();
    
    py::enum_<GeoJSON::Type>(m, "Type")
        .value("POINT", GeoJSON::Type::POINT)
        .value("MULTI_POINT", GeoJSON::Type::MULTI_POINT)
        .value("LINE_STRING", GeoJSON::Type::LINE_STRING)
        .value("MULTI_LINE_STRING", GeoJSON::Type::MULTI_LINE_STRING)
        .value("POLYGON", GeoJSON::Type::POLYGON)
        .value("MULTI_POLYGON", GeoJSON::Type::MULTI_POLYGON)
        .value("GEOMETRY_COLLECTION", GeoJSON::Type::GEOMETRY_COLLECTION)
        .value("FEATURE_COLLECTION", GeoJSON::Type::FEATURE_COLLECTION)
        .value("FEATURE", GeoJSON::Type::FEATURE)
        .value("UNKNOWN", GeoJSON::Type::UNKNOWN)
        .export_values();
    
    // Utility functions
    m.def("string_to_type", &GeoJSON::String_To_Type, "Convert string to Type enum");
    m.def("string_to_key", &GeoJSON::String_To_Key, "Convert string to Key enum");
}

#endif //PYBIND_IO_H