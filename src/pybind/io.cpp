#include "pybind/io.h"

// PYBIND
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

// STL
#include <filesystem>

// RAPIDJSON
#include <rapidjson/filereadstream.h>
using namespace O;

template<class T>
GeoJSON::IO::Error Parse_From_File(T& handler, const std::filesystem::path& path)
{
	auto fp = std::unique_ptr<FILE, decltype(&fclose)>(fopen(path.string().c_str(), "r"), fclose);
	if (!fp)  return GeoJSON::IO::Error::FILE_OPENNING_FAILED;

	char readBuffer[65536];
	rapidjson::FileReadStream is(fp.get(), readBuffer, sizeof(readBuffer));

	rapidjson::Reader reader;

	if (!reader.Parse(is, handler))
	{
		if (handler.Get_Error() != GeoJSON::IO::Error::NO_ERROR)
			return handler.Get_Error();
		else
			return GeoJSON::IO::Error::PARSING_ERROR;
	}
	return GeoJSON::IO::Error::NO_ERROR;
}

template<class T>
GeoJSON::IO::Error Parse_From_String(T&handler, const std::string& str)
{
	rapidjson::Reader reader;
	rapidjson::StringStream ss(str.c_str());

	if (!reader.Parse(ss, handler))
	{
		if (handler.Get_Error() != GeoJSON::IO::Error::NO_ERROR)
			return handler.Get_Error();
		else
			return GeoJSON::IO::Error::PARSING_ERROR;
	}
	return GeoJSON::IO::Error::NO_ERROR;
}

GeoJSON::IO::Error Py_Feature_Parser::Parse_From_File(const std::filesystem::path& path)
{
	return ::Parse_From_File<Py_Feature_Parser>(*this, path);
}

GeoJSON::IO::Error Py_Feature_Parser::Parse_From_String(const std::string& str)
{
	return ::Parse_From_String<Py_Feature_Parser>(*this, str);
}

GeoJSON::IO::Error Py_SAX_Parser::Parse_From_File(const std::filesystem::path& path)
{
	return ::Parse_From_File<Py_SAX_Parser>(*this, path);
}

GeoJSON::IO::Error Py_SAX_Parser::Parse_From_String(const std::string& str)
{
	return ::Parse_From_String<Py_SAX_Parser>(*this, str);
}

GeoJSON::IO::Error Py_Full_Parser::Parse_From_File(const std::filesystem::path& path)
{
	auto ret =  ::Parse_From_File<Py_Full_Parser>(*this, path);
	if (ret != GeoJSON::IO::Error::NO_ERROR) return ret;
	if (auto geojson = this->Get_Geojson())
		m_geojson = std::move(*geojson);
	if (this->Get_Error() != GeoJSON::IO::Error::NO_ERROR)
		return this->Get_Error();
	else
		return GeoJSON::IO::Error::PARSING_ERROR;
}

GeoJSON::IO::Error Py_Full_Parser::Parse_From_String(const std::string& str)
{
	auto ret = ::Parse_From_String<Py_Full_Parser>(*this, str);
	if (ret != GeoJSON::IO::Error::NO_ERROR) return ret;
	if (ret != GeoJSON::IO::Error::NO_ERROR) return ret;
	if (auto geojson = this->Get_Geojson())
		m_geojson = std::move(*geojson);
	if (this->Get_Error() != GeoJSON::IO::Error::NO_ERROR)
		return this->Get_Error();
	else
		return GeoJSON::IO::Error::PARSING_ERROR;
}

O::GeoJSON::Root Py_Full_Parser::Get_Value()
{
	return  m_geojson;
}

void Init_Io_Bindings(pybind11::module_ &m)
{
	// Error Type enum
	pybind11::enum_<GeoJSON::IO::Error>(m, "Error")
		.value("NO_ERROR",                                    GeoJSON::IO::Error::NO_ERROR)
		.value("COORDINATE_UNDERSIZED",                       GeoJSON::IO::Error::COORDINATE_UNDERSIZED)
		.value("COORDINATE_OVERSIZED",                        GeoJSON::IO::Error::COORDINATE_OVERSIZED)
		.value("INCONSCISTENT_COORDINATE_LEVEL",              GeoJSON::IO::Error::INCONSCISTENT_COORDINATE_LEVEL)
		.value("FEATURE_NEED_INITIALIZED_GEOMETRY",           GeoJSON::IO::Error::FEATURE_NEED_INITIALIZED_GEOMETRY)
		.value("BAD_COORDINATE_FOR_GEMETRY",                  GeoJSON::IO::Error::BAD_COORDINATE_FOR_GEMETRY)
		.value("NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING",   GeoJSON::IO::Error::NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING)
		.value("NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON",     GeoJSON::IO::Error::NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON)
		.value("POLYGON_NEED_TO_BE_CLOSED",                   GeoJSON::IO::Error::POLYGON_NEED_TO_BE_CLOSED)
		.value("POLYGON_NEED_AT_LEAST_ONE_RING",              GeoJSON::IO::Error::POLYGON_NEED_AT_LEAST_ONE_RING)
		.value("UNKNOWN_GEOMETRY_TYPE",                       GeoJSON::IO::Error::UNKNOWN_GEOMETRY_TYPE)
		.value("UNKNOWN_TYPE",                                GeoJSON::IO::Error::UNKNOWN_TYPE)
		.value("UNEXPECTED_STATE_KEY",                        GeoJSON::IO::Error::UNEXPECTED_STATE_KEY)
		.value("UNEXPECTED_STATE_VALUE",                      GeoJSON::IO::Error::UNEXPECTED_STATE_VALUE)
		.value("UNEXPECTED_STATE_OBJECT",                     GeoJSON::IO::Error::UNEXPECTED_STATE_OBJECT)
		.value("UNEXPECTED_STATE_ARRAY",                      GeoJSON::IO::Error::UNEXPECTED_STATE_ARRAY)
		.value("UNEXPECTED_PROPERTY_STATE",                   GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE)
		.value("BBOX_SIZE_INCONSISTENT",                      GeoJSON::IO::Error::BBOX_SIZE_INCONSISTENT)
		.value("PARSING_ERROR",                               GeoJSON::IO::Error::PARSING_ERROR)
		.value("FILE_OPENNING_FAILED",                        GeoJSON::IO::Error::FILE_OPENNING_FAILED)
		.value("PROPERTY_KEY_ALREADY_EXIST",                  GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST)
		.value("RECURSIVE_GEMETRY_COLLECTION_UNSUPPORTED",    GeoJSON::IO::Error::RECURSIVE_GEMETRY_COLLECTION_UNSUPPORTED)
		.value("UNKNOWN_ROOT_OBJECT",                         GeoJSON::IO::Error::UNKNOWN_ROOT_OBJECT)
		.value("GEOMETRY_COLLECTION_ELLEMENT_COUNT_MISMATCH", GeoJSON::IO::Error::GEOMETRY_COLLECTION_ELLEMENT_COUNT_MISMATCH
		).export_values();
	
	// FullParser
	pybind11::class_<Py_Full_Parser>(m, "FullParser")
		.def(pybind11::init<>())
		.def("Parse_From_File",   &Py_Full_Parser::Parse_From_File)
		.def("Parse_From_String", &Py_Full_Parser::Parse_From_String)
		.def("Get_Geojson",       &Py_Full_Parser::Get_Value);

	
	// FeatureParser (trampoline class for Python inheritance)
	pybind11::class_<Py_Feature_Parser>(m, "Feature_Parser")
		.def(pybind11::init<>())
		.def("Parse_From_File", &Py_Feature_Parser::Parse_From_File)
		.def("Parse_From_String", &Py_Feature_Parser::Parse_From_String);
	
	// SAXParser (trampoline class for Python inheritance)  
	pybind11::class_<Py_SAX_Parser>(m, "Sax_Parser")
		.def(pybind11::init<>())
		.def("Parse_From_File", &Py_SAX_Parser::Parse_From_File)
		.def("Parse_From_String", &Py_SAX_Parser::Parse_From_String);

	pybind11::class_<std::filesystem::path>(m, "Path")
		.def(pybind11::init<std::string>());
	pybind11::implicitly_convertible<std::string, std::filesystem::path>();
}