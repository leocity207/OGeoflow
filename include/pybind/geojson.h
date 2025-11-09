#ifndef PYBIND_GEOJSON_H
#define PYBIND_GEOJSON_H

#include <pybind11/pybind11.h>

void Init_Io_Bindings(py::module_ &m) {
    // Error Type enum
    py::enum_<GeoJSON::IO::Error::Type>(m, "ErrorType")
        .value("NO_ERROR", GeoJSON::IO::Error::Type::NO_ERROR)
        .value("COORDINATE_UNDERSIZED", GeoJSON::IO::Error::Type::COORDINATE_UNDERSIZED)
        .value("COORDINATE_OVERSIZED", GeoJSON::IO::Error::Type::COORDINATE_OVERSIZED)
        .value("INCONSCISTENT_COORDINATE_LEVEL", GeoJSON::IO::Error::Type::INCONSCISTENT_COORDINATE_LEVEL)
        .value("FEATURE_NEED_INITIALIZED_GEOMETRY", GeoJSON::IO::Error::Type::FEATURE_NEED_INITIALIZED_GEOMETRY)
        .value("BAD_COORDINATE_FOR_GEMETRY", GeoJSON::IO::Error::Type::BAD_COORDINATE_FOR_GEMETRY)
        .value("NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING", GeoJSON::IO::Error::Type::NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING)
        .value("NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON", GeoJSON::IO::Error::Type::NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON)
        .value("POLYGON_NEED_TO_BE_CLOSED", GeoJSON::IO::Error::Type::POLYGON_NEED_TO_BE_CLOSED)
        .value("POLYGON_NEED_AT_LEAST_ONE_RING", GeoJSON::IO::Error::Type::POLYGON_NEED_AT_LEAST_ONE_RING)
        .value("UNKNOWN_GEOMETRY_TYPE", GeoJSON::IO::Error::Type::UNKNOWN_GEOMETRY_TYPE)
        .value("UNKNOWN_TYPE", GeoJSON::IO::Error::Type::UNKNOWN_TYPE)
        .value("UNEXPECTED_STATE_KEY", GeoJSON::IO::Error::Type::UNEXPECTED_STATE_KEY)
        .value("UNEXPECTED_STATE_VALUE", GeoJSON::IO::Error::Type::UNEXPECTED_STATE_VALUE)
        .value("UNEXPECTED_STATE_OBJECT", GeoJSON::IO::Error::Type::UNEXPECTED_STATE_OBJECT)
        .value("UNEXPECTED_STATE_ARRAY", GeoJSON::IO::Error::Type::UNEXPECTED_STATE_ARRAY)
        .value("UNEXPECTED_PROPERTY_STATE", GeoJSON::IO::Error::Type::UNEXPECTED_PROPERTY_STATE)
        .value("BBOX_SIZE_INCONSISTENT", GeoJSON::IO::Error::Type::BBOX_SIZE_INCONSISTENT)
        .value("PARSING_ERROR", GeoJSON::IO::Error::Type::PARSING_ERROR)
        .value("FILE_OPENNING_FAILED", GeoJSON::IO::Error::Type::FILE_OPENNING_FAILED)
        .value("PROPERTY_KEY_ALREADY_EXIST", GeoJSON::IO::Error::Type::PROPERTY_KEY_ALREADY_EXIST)
        .value("RECURSIVE_GEMETRY_COLLECTION_UNSUPPORTED", GeoJSON::IO::Error::Type::RECURSIVE_GEMETRY_COLLECTION_UNSUPPORTED)
        .value("UNKNOWN_ROOT_OBJECT", GeoJSON::IO::Error::Type::UNKNOWN_ROOT_OBJECT)
        .value("GEOMETRY_COLLECTION_ELEMENT_COUNT_MISMATCH", GeoJSON::IO::Error::Type::GEOMETRy_COLLECTION_ELLEMENT_COUNT_MISMATCH)
        .export_values();
    
    // FullParser
    py::class_<GeoJSON::IO::Full_Parser>(m, "FullParser")
        .def(py::init<>())
        .def("get_geojson", &GeoJSON::IO::Full_Parser::Get_Geojson)
        .def("on_geometry", &GeoJSON::IO::Full_Parser::On_Geometry)
        .def("on_feature", &GeoJSON::IO::Full_Parser::On_Feature)
        .def("on_feature_collection", &GeoJSON::IO::Full_Parser::On_Feature_Collection);
    
    // FeatureParser (trampoline class for Python inheritance)
    py::class_<GeoJSON::IO::Feature_Parser<PyFeatureParser>, PyFeatureParser>(m, "FeatureParser")
        .def(py::init<>())
        .def("on_full_feature", &GeoJSON::IO::Feature_Parser<PyFeatureParser>::On_Full_Feature)
        .def("on_root", &GeoJSON::IO::Feature_Parser<PyFeatureParser>::On_Root);
    
    // SAXParser (trampoline class for Python inheritance)  
    py::class_<GeoJSON::IO::SAX_Parser<PySAXParser>, PySAXParser>(m, "SAXParser")
        .def(py::init<>())
        .def("on_geometry", &GeoJSON::IO::SAX_Parser<PySAXParser>::On_Geometry)
        .def("on_feature", &GeoJSON::IO::SAX_Parser<PySAXParser>::On_Feature)
        .def("on_feature_collection", &GeoJSON::IO::SAX_Parser<PySAXParser>::On_Feature_Collection)
        .def("push_error", &GeoJSON::IO::SAX_Parser<PySAXParser>::Push_Error);
    
    // Parse functions
    m.def("parse_geojson_string", &GeoJSON::IO::Parse_Geojson_String, 
          "Parse GeoJSON from string", py::arg("json_string"));
    
    m.def("parse_geojson_file", &GeoJSON::IO::Parse_Geojson_File, 
          "Parse GeoJSON from file", py::arg("filename"));
}

#endif //PYBIND_GEOJSON_H
