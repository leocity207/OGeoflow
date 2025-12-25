#ifndef PYBIND_IO_H
#define PYBIND_IO_H

// PYBIND
#include <pybind11/pybind11.h>

// IO
#include "io/error.h"
#include "io/full_parser.h"
#include "io/feature_parser.h"
#include "io/sax_parser.h"


void Init_Io_Bindings(pybind11::module_ &m);


class Py_Feature_Parser : public O::GeoJSON::IO::Feature_Parser<Py_Feature_Parser>
{
public:
	using Base = O::GeoJSON::IO::Feature_Parser<Py_Feature_Parser>;
	

	O::GeoJSON::IO::Error Parse_From_File(const std::filesystem::path& path);
	O::GeoJSON::IO::Error Parse_From_String(const std::string& str);

	bool On_Full_Feature(O::GeoJSON::Feature&& feature)
	{
		PYBIND11_OVERRIDE(bool, Base, On_Full_Feature, std::move(feature));
	}

	bool On_Root(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
	{
		PYBIND11_OVERRIDE(bool, Base, On_Root, std::move(bbox), std::move(id) );
	}
};

class Py_SAX_Parser : public O::GeoJSON::IO::SAX_Parser<Py_SAX_Parser>
{
public:
	using Base = O::GeoJSON::IO::SAX_Parser<Py_SAX_Parser>;

	O::GeoJSON::IO::Error Parse_From_File(const std::filesystem::path& path);
	O::GeoJSON::IO::Error Parse_From_String(const std::string& str);

	bool On_Geometry(O::GeoJSON::Geometry&& g, size_t i)
	{
		PYBIND11_OVERRIDE(bool, Base, On_Geometry, std::move(g), i);
	}

	bool On_Feature(O::GeoJSON::Feature&& f)
	{
		PYBIND11_OVERRIDE(bool, Base, On_Feature, std::move(f));
	}

	bool On_Feature_Collection(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
	{
		PYBIND11_OVERRIDE(bool, Base, On_Feature_Collection, std::move(bbox), std::move(id));
	}
};

class Py_Full_Parser : public O::GeoJSON::IO::Full_Parser
{
public:
	O::GeoJSON::IO::Error Parse_From_File(const std::filesystem::path& path);
	O::GeoJSON::IO::Error Parse_From_String(const std::string& str);
	O::GeoJSON::Root Get_Value();
private:
	O::GeoJSON::Root m_geojson;
};

#endif //PYBIND_IO_H
