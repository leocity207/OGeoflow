#ifndef PYBIND_MAIN_H
#define PYBIND_MAIN_H

// PYBIND11
#include <pybind11/pybind11.h>

// PYBIND
#include "geojson.h"
#include "io.h"
#include "dcel.h"

PYBIND11_MODULE(geojson_parser, m) {
	m.doc() = "GeoJSON parser and DCEL Construction";
	
	// Create submodules for better organization
	auto m_geojson = m.def_submodule("geojson", "Core GeoJSON types");
	auto m_io = m.def_submodule("io", "GeoJSON I/O operations");
	
	Init_Geojson_Bindings(m_geojson);
	Init_Io_Bindings(m_io);
}

#endif //PYBIND_MAIN_H