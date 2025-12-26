#ifndef PYBIND_GEOJSON_H
#define PYBIND_GEOJSON_H

// PYBIND
#include <pybind11/pybind11.h>

// GEOJSON
#include "geojson/properties.h"
#include "geojson/root.h"
#include "geojson/bbox.h"
#include "geojson/position.h"
#include "geojson/geometry_type/geometry_collection.h"
#include "geojson/geometry_type/line_string.h"
#include "geojson/geometry_type/multi_line_string.h"
#include "geojson/geometry_type/point.h"
#include "geojson/geometry_type/multi_point.h"
#include "geojson/geometry_type/polygon.h"
#include "geojson/geometry_type/multi_polygon.h"
#include "geojson/object/feature.h"
#include "geojson/object/feature_collection.h"
#include "geojson/object/geometry.h"


pybind11::object Property_To_PyBind(const O::GeoJSON::Property& p);
O::GeoJSON::Property PyBind_To_Property(pybind11::handle obj);

void Init_Geojson_Bindings(pybind11::module_& m);


template<class Class, class Type>
void Setter_Object(Class& c, Type t)
{
	c.object = std::move(t);
}

template<class Class, class Type>
Type Getter_Object(const  Class& c)
{
	return c.object;
}



#endif //PYBIND_GEOJSON_H