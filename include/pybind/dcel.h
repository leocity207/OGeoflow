#ifndef PYBIND_DCEL_H
#define PYBIND_IO_H

// PYBIND11
#include <pybind11/pybind11.h>

// CONFIGURATION
#include "configuration/dcel.h"
#include "dcel/builder.h"
#include "dcel/storage.h"
#include "dcel/feature_info.h"
#include "dcel/vertex.h"
#include "dcel/face.h"
#include "dcel/half_edge.h"

// STL
#include <filesystem>

// IO
#include "io/error.h"

struct Half_Edge_Impl : public O::DCEL::Half_Edge<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>> {
	using O::DCEL::Half_Edge<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>>::Half_Edge;
};

void Init_DCEL_Bindings(pybind11::module_ &m);

class Py_DCEL_Builder
{
public:
	Py_DCEL_Builder(const O::Configuration::DCEL& config);

	bool Parse_Root(O::GeoJSON::Root& geojson);

	O::GeoJSON::IO::Error Parse_File(const std::filesystem::path& path);
	O::GeoJSON::IO::Error Parse_String(const std::string& path);

	O::DCEL::Storage<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl , O::DCEL::Face<Half_Edge_Impl>>& Get_DCEL_Ref();
	O::DCEL::Feature_Info<O::DCEL::Face<Half_Edge_Impl>>& Get_Feature_Info_Ref();

private:
	O::Configuration::DCEL m_config;
	O::DCEL::Storage<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>> m_storage;
	O::DCEL::Feature_Info<O::DCEL::Face<Half_Edge_Impl>> m_features_infos;
};


class Py_Feature_Predicate
{
public:
	Py_Feature_Predicate(pybind11::function fn);
	bool operator()(O::GeoJSON::Feature& feature);

private:
	pybind11::function m_predicate_function;
};

class Py_DCEL_Builder_With_Filter
{
public:
	Py_DCEL_Builder_With_Filter(const O::Configuration::DCEL& cfg, pybind11::function predicate);

	O::GeoJSON::IO::Error Parse_File(const std::filesystem::path& path);
	O::GeoJSON::IO::Error Parse_String(const std::string& path);

	O::DCEL::Storage<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>>& Get_DCEL_Ref();
	O::DCEL::Feature_Info<O::DCEL::Face<Half_Edge_Impl>>& Get_Feature_Info_Ref();

private:
	O::Configuration::DCEL m_config;
    O::DCEL::Storage<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>> m_storage;
    O::DCEL::Feature_Info<O::DCEL::Face<Half_Edge_Impl>> m_features_infos;
	Py_Feature_Predicate m_filter_predicate;
};

#endif //PYBIND_DCEL_H