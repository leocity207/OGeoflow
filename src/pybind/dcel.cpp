#include "pybind/dcel.h"

using namespace O;

// PYBIND
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

// RAPIDJSON
#include <rapidjson/filereadstream.h>

// Filter
#include "filter/feature.h"


template<class T>
using Filtered_Builder = O::GeoJSON::Filter::Feature< T, Py_Feature_Predicate>;

class Auto_Builder : public O::DCEL::Builder::From_GeoJSON, public O::GeoJSON::IO::Feature_Parser<Auto_Builder> {
public:
	using O::DCEL::Builder::From_GeoJSON::On_Full_Feature;
	using O::DCEL::Builder::From_GeoJSON::On_Root;
	Auto_Builder(const O::Configuration::DCEL& conf) :
		O::DCEL::Builder::From_GeoJSON(conf),
		O::GeoJSON::IO::Feature_Parser<Auto_Builder>()
	{

	}
};

class Auto_Filtered_Builder : public O::DCEL::Builder::From_GeoJSON, public Filtered_Builder<Auto_Filtered_Builder> {
public:
	using O::DCEL::Builder::From_GeoJSON::On_Full_Feature;
	using O::DCEL::Builder::From_GeoJSON::On_Root;
	Auto_Filtered_Builder(const O::Configuration::DCEL& conf, Py_Feature_Predicate predicate) :
		O::DCEL::Builder::From_GeoJSON(conf),
		Filtered_Builder<Auto_Filtered_Builder>( predicate)
	{

	}
};
// Py_DCEL_Builder

Py_DCEL_Builder::Py_DCEL_Builder(const O::Configuration::DCEL& config):
	m_config(config),
	m_storage(config),
	m_features_infos()
{

}

bool Py_DCEL_Builder::Parse_Root(O::GeoJSON::Root& geojson)
{
	O::DCEL::Builder::From_GeoJSON builder(m_config);
	return builder.Parse(std::move(geojson));
}

GeoJSON::IO::Error Py_DCEL_Builder::Parse_File(const std::filesystem::path& path)
{
	auto fp = std::unique_ptr<FILE,decltype(&fclose)>(fopen(path.string().c_str(), "r"),fclose);
	if(!fp)  return GeoJSON::IO::Error::FILE_OPENNING_FAILED;
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp.get(), readBuffer, sizeof(readBuffer));
	rapidjson::Reader reader;
	Auto_Builder auto_builder(m_config);
	if(reader.Parse(is, auto_builder))
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	auto opt_dcel = auto_builder.Get_Dcel();
	if(!opt_dcel.has_value())
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	m_storage = opt_dcel.value();
	auto opt_feature_info = auto_builder.Get_Feature_Info();
	if(!opt_feature_info.has_value())
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	m_features_infos = opt_feature_info.value();
	return GeoJSON::IO::Error::NO_ERROR;
}

GeoJSON::IO::Error Py_DCEL_Builder::Parse_String(const std::string& str)
{
	Auto_Builder auto_builder(m_config);
	rapidjson::StringStream ss(str.c_str());
	rapidjson::Reader reader;
	if(reader.Parse(ss, auto_builder))
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	auto opt_dcel = auto_builder.Get_Dcel();
	if(!opt_dcel.has_value())
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	m_storage = opt_dcel.value();
		auto opt_feature_info = auto_builder.Get_Feature_Info();
	if(!opt_feature_info.has_value())
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	m_features_infos = opt_feature_info.value();
	return GeoJSON::IO::Error::NO_ERROR;
}

O::DCEL::Storage& Py_DCEL_Builder::Get_DCEL_Ref()
{
	return m_storage;
}

O::DCEL::Feature_Info& Py_DCEL_Builder::Get_Feature_Info_Ref()
{
	return m_features_infos;
}

Py_Feature_Predicate::Py_Feature_Predicate(pybind11::function fn):
	m_predicate_function(std::move(fn))
{

}

bool Py_Feature_Predicate::operator()(O::GeoJSON::Feature& feature)
{
	pybind11::gil_scoped_acquire gil;
	return m_predicate_function(feature).cast<bool>();
}

// Py_DCEL_Builder_With_Filter

Py_DCEL_Builder_With_Filter::Py_DCEL_Builder_With_Filter(const O::Configuration::DCEL& cfg, pybind11::function predicate) :
	m_config(cfg),
	m_storage(cfg),
	m_features_infos(),
	m_filter_predicate(Py_Feature_Predicate(predicate))
{

}

GeoJSON::IO::Error Py_DCEL_Builder_With_Filter::Parse_File(const std::filesystem::path& path)
{
	auto fp = std::unique_ptr<FILE,decltype(&fclose)>(fopen(path.string().c_str(), "r"),fclose);
	if(!fp)  return GeoJSON::IO::Error::FILE_OPENNING_FAILED;
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp.get(), readBuffer, sizeof(readBuffer));
	rapidjson::Reader reader;
	Auto_Filtered_Builder auto_builder(m_config, m_filter_predicate);
	if(reader.Parse(is, auto_builder))
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	auto opt_dcel = auto_builder.Get_Dcel();
	if(!opt_dcel.has_value())
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	m_storage = opt_dcel.value();
	auto opt_feature_info = auto_builder.Get_Feature_Info();
	if(!opt_feature_info.has_value())
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	m_features_infos = opt_feature_info.value();
	return GeoJSON::IO::Error::NO_ERROR;
}

GeoJSON::IO::Error Py_DCEL_Builder_With_Filter::Parse_String(const std::string& str)
{
	Auto_Filtered_Builder auto_builder(m_config, m_filter_predicate);
	rapidjson::StringStream ss(str.c_str());
	rapidjson::Reader reader;
	if(reader.Parse(ss, auto_builder))
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	auto opt_dcel = auto_builder.Get_Dcel();
	if(!opt_dcel.has_value())
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	m_storage = opt_dcel.value();
		auto opt_feature_info = auto_builder.Get_Feature_Info();
	if(!opt_feature_info.has_value())
		return (auto_builder.Get_Error() != GeoJSON::IO::Error::NO_ERROR) ? auto_builder.Get_Error() : GeoJSON::IO::Error::PARSING_ERROR;
	m_features_infos = opt_feature_info.value();
	return GeoJSON::IO::Error::NO_ERROR;
}

O::DCEL::Storage& Py_DCEL_Builder_With_Filter::Get_DCEL_Ref()
{
	return m_storage;
}

O::DCEL::Feature_Info& Py_DCEL_Builder_With_Filter::Get_Feature_Info_Ref()
{
	return m_features_infos;
}

void Init_DCEL_Bindings(pybind11::module_ &m)
{
	pybind11::class_<Py_DCEL_Builder>(m, "DCEL_Builder")
		.def(pybind11::init<const O::Configuration::DCEL&>())
		.def("Parse",                &Py_DCEL_Builder::Parse_Root)
		.def("Parse_GeoJSON_File",   &Py_DCEL_Builder::Parse_File)
		.def("Parse_GeoJSON_String", &Py_DCEL_Builder::Parse_String)
		.def("DCEL",                 &Py_DCEL_Builder::Get_DCEL_Ref)
		.def("Feature_Info",         &Py_DCEL_Builder::Get_Feature_Info_Ref);

	pybind11::class_<Py_DCEL_Builder_With_Filter>(m, "DCEL_Filtered_Builder")
		.def(pybind11::init<const O::Configuration::DCEL&, pybind11::function>(), pybind11::arg("config"), pybind11::arg("predicate"))
		.def("Parse_GeoJSON_File",   &Py_DCEL_Builder_With_Filter::Parse_File)
		.def("Parse_GeoJSON_String", &Py_DCEL_Builder_With_Filter::Parse_String)
		.def("DCEL",                 &Py_DCEL_Builder_With_Filter::Get_DCEL_Ref)
		.def("Feature_Info",         &Py_DCEL_Builder_With_Filter::Get_Feature_Info_Ref);

	pybind11::class_<DCEL::Vertex>(m, "Vertex")
		.def_property_readonly("x",              [](const DCEL::Vertex& v) { return v.x; })
		.def_property_readonly("y",              [](const DCEL::Vertex& v) { return v.y; })
		.def_property_readonly("outgoing_edges", [](const DCEL::Vertex& v) { return v.outgoing_edges;},pybind11::return_value_policy::reference_internal);

	pybind11::class_<DCEL::Half_Edge>(m, "HalfEdge")
		.def_property_readonly("tail", [](const DCEL::Half_Edge& e) { return e.tail; },pybind11::return_value_policy::reference_internal)
		.def_property_readonly("head", [](const DCEL::Half_Edge& e) { return e.head; },pybind11::return_value_policy::reference_internal)
		.def_property_readonly("twin", [](const DCEL::Half_Edge& e) { return e.twin; },pybind11::return_value_policy::reference_internal)
		.def_property_readonly("next", [](const DCEL::Half_Edge& e) { return e.next; },pybind11::return_value_policy::reference_internal)
		.def_property_readonly("prev", [](const DCEL::Half_Edge& e) { return e.prev; },pybind11::return_value_policy::reference_internal)
		.def_property_readonly("face", [](const DCEL::Half_Edge& e) { return e.face; },pybind11::return_value_policy::reference_internal);
	
	pybind11::class_<DCEL::Face>(m, "Face")
		.def_property_readonly("edge", [](const DCEL::Face& f) { return f.edge; }, pybind11::return_value_policy::reference_internal);

	pybind11::class_<DCEL::Storage>(m, "Storage")
		.def_property_readonly("vertices",  [](DCEL::Storage& s) { return s.vertices;},   pybind11::return_value_policy::reference_internal)
		.def_property_readonly("half_edges",[](DCEL::Storage& s) { return s.half_edges;}, pybind11::return_value_policy::reference_internal)
		.def_property_readonly("faces",     [](DCEL::Storage& s) { return s.faces;},      pybind11::return_value_policy::reference_internal);

	pybind11::enum_<Configuration::DCEL::Merge_Strategy>(m, "Merge_Strategy")
		.value("AT_FIRST",                                    Configuration::DCEL::Merge_Strategy::AT_FIRST);
	
	pybind11::class_<Configuration::DCEL>(m, "Configuration")
		.def(pybind11::init<>())
		.def_readwrite("max_vertices",          &Configuration::DCEL::max_vertices)
		.def_readwrite("max_half_edges",        &Configuration::DCEL::max_half_edges)
		.def_readwrite("max_faces",             &Configuration::DCEL::max_faces)
		.def_readwrite("position_tolerance",    &Configuration::DCEL::position_tolerance)
		.def_readwrite("vertex_merge_strategy", &Configuration::DCEL::vertex_merge_strategy);
}