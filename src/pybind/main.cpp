// PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

// PYBIND
#include "pybind/geojson.h"
#include "pybind/io.h"
#include "pybind/dcel.h"

PYBIND11_MODULE(pygeoflow, m)
{
	m.doc() = "GeoJSON parser and DCEL Construction";
	
	// Create submodules for better organization
	auto m_geojson = m.def_submodule("geojson", "Core GeoJSON types");
	auto m_io = m.def_submodule("io", "GeoJSON I/O operations");
	auto m_dcel = m.def_submodule("dcel", "Double Connected Edge List operations");

	Init_Geojson_Bindings(m_geojson);
	Init_Io_Bindings(m_io);
	Init_DCEL_Bindings(m_dcel);
}