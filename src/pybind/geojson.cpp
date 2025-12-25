#include "pybind/geojson.h"

// PYBIND
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

#include "geojson/properties.h"

using namespace O::GeoJSON;

pybind11::object Property_To_PyBind(const Property& p)
{
    using Array = Property::Array;
    using Object = Property::Object;

    if (p.Is_Null()) return pybind11::none();
    if (p.Is_Bool()) return pybind11::bool_(p.Get_Bool());
    if (p.Is_Integer()) return pybind11::int_(p.Get_Int());
    if (p.Is_Double()) return pybind11::float_(p.Get_Double());
    if (p.Is_String()) return pybind11::str(std::string(p.Get_String()));
    if (p.Is_Array()) {
        const Array & a = p.Get_Array();
        pybind11::list l;
        for (const auto& el : a) 
			l.append(Property_To_PyBind(el));
        return l;
    }
    if (p.Is_Object()) {
        const Object& o = p.Get_Object();
        pybind11::dict d;
        for (const auto& kv : o)
            d[pybind11::str(kv.first)] = Property_To_PyBind(kv.second);
        return d;
    }
    // fallback (shouldn't happen)
    return pybind11::none();
}

inline Property PyBind_To_Property(pybind11::handle obj)
{
    using Array = Property::Array;
    using Object = Property::Object;

    Property prop;

    if (obj.is_none())
	{
        prop = nullptr;
        return prop;
    }
    if (pybind11::isinstance<pybind11::bool_>(obj))
	{
        prop = obj.cast<bool>();
        return prop;
    }
    if (pybind11::isinstance<pybind11::int_>(obj))
	{
        try {
            long long v = obj.cast<long long>();
            prop = static_cast<std::int64_t>(v);
            return prop;
        } catch (const std::exception&) {
            // integer too large for int64_t: fall back to double (lossy but safe)
            double dv = obj.cast<double>();
            prop = dv;
            return prop;
        }
    }
    if (pybind11::isinstance<pybind11::float_>(obj))
	{
        prop = obj.cast<double>();
        return prop;
    }
    if (pybind11::isinstance<pybind11::str>(obj))
	{
        prop = obj.cast<std::string>();
        return prop;
    }
    if (pybind11::isinstance<pybind11::sequence>(obj))
	{
        Array arr;
        pybind11::sequence seq = obj.cast<pybind11::sequence>();
        arr.reserve(seq.size());
        for (pybind11::handle item : seq) {
            arr.push_back(PyBind_To_Property(item));
        }
        prop = std::move(arr);
        return prop;
    }
    if (pybind11::isinstance<pybind11::dict>(obj))
	{
        Object mp;
        pybind11::dict d = obj.cast<pybind11::dict>();
        for (auto item : d) {
            // key must be string
            std::string key = pybind11::str(item.first).cast<std::string>();
            pybind11::handle val = item.second;
            mp.emplace(std::move(key), PyBind_To_Property(val));
        }
        prop = std::move(mp);
        return prop;
    }
    throw std::runtime_error("Unsupported pybind11thon type for conversion to GeoJSON::Property");
}

void Init_Geojson_Bindings(pybind11::module_& m)
{
	// Position
	pybind11::class_<Position>(m, "Position")
		.def(pybind11::init<>())
		.def(pybind11::init<double, double>())
		.def(pybind11::init<double, double, std::optional<double>>())
		.def_readwrite("longitude", &Position::longitude)
		.def_readwrite("latitude", &Position::latitude)
		.def_readwrite("altitude", &Position::altitude)
		.def("__repr__", [](const Position& p) {
			if (p.altitude)
				return "Position(longitude=" + std::to_string(p.longitude) + ", latitude=" + std::to_string(p.latitude) + ", altitude=" + std::to_string(*p.altitude) + ")";
			return "Position(longitude=" + std::to_string(p.longitude) + ", latitude=" + std::to_string(p.latitude) + ")";
		});

	// Bbox
	pybind11::class_<Bbox>(m, "Bbox")
		.def(pybind11::init<>())
		.def("Has_Altitude", &Bbox::Has_Altitude)
		.def("Get",                [](const Bbox& b) { return b.Get(); })
		.def("Get_With_Altitudes", [](const Bbox& b) { return b.Get_With_Altitudes(); })
		.def("__repr__", [](const Bbox& b) {
			return "Bbox(Has_Altitude=" + std::to_string(b.Has_Altitude()) + ")";
		});

	// Property
	pybind11::class_<Property>(m, "Property")
		.def(pybind11::init<>())
		.def(pybind11::init<bool>())
		.def(pybind11::init<int>())
		.def(pybind11::init<int64_t>())
		.def(pybind11::init<double>())
		.def(pybind11::init<const char*>())
		.def(pybind11::init<std::string>())
		.def("Get", [](const Property& p) {return Property_To_PyBind(p); })
		.def("Set", [](Property& p, const pybind11::handle& obj) {p = PyBind_To_Property(obj); })
		.def("__repr__", [](const Property& p) -> std::string{
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
	pybind11::class_<Point>(m, "Point")
		.def(pybind11::init<>())
		.def_readwrite("position", &Point::position)
		.def("__repr__", [](const Point& p) {
			return "Point(" + std::to_string(p.position.longitude) + ", " + std::to_string(p.position.latitude) + ")";
		});

	// LineString
	pybind11::class_<Line_String>(m, "LineString")
		.def(pybind11::init<>())
		.def_readwrite("positions", &Line_String::positions)
		.def("__repr__", [](const Line_String& ls) {
			return "LineString(positions=" + std::to_string(ls.positions.size()) + ")";
		});

	// Polygon
	pybind11::class_<Polygon>(m, "Polygon")
		.def(pybind11::init<>())
		.def_readwrite("rings", &Polygon::rings)
		.def("__repr__", [](const Polygon& p) {
			return "Polygon(rings=" + std::to_string(p.rings.size()) + ")";
		});

	// MultiPoint
	pybind11::class_<Multi_Point>(m, "MultiPoint")
		.def(pybind11::init<>())
		.def_readwrite("points", &Multi_Point::points)
		.def("__repr__", [](const Multi_Point& mp) {
		return "MultiPoint(points=" + std::to_string(mp.points.size()) + ")";
			});

	// MultiLineString
	pybind11::class_<Multi_Line_String>(m, "MultiLineString")
		.def(pybind11::init<>())
		.def_readwrite("line_strings", &Multi_Line_String::line_strings)
		.def("__repr__", [](const Multi_Line_String& mls) {
			return "MultiLineString(line_strings=" + std::to_string(mls.line_strings.size()) + ")";
		});

	// MultiPolygon
	pybind11::class_<Multi_Polygon>(m, "MultiPolygon")
		.def(pybind11::init<>())
		.def_readwrite("polygons", &Multi_Polygon::polygons)
		.def("__repr__", [](const Multi_Polygon& mp) {
			return "MultiPolygon(polygons=" + std::to_string(mp.polygons.size()) + ")";
		});

	// GeometryCollection
	pybind11::class_<Geometry_Collection>(m, "Geometry_Collection")
		.def(pybind11::init<>())
		.def_readwrite("geometries", &Geometry_Collection::geometries)
		.def("__repr__", [](const Geometry_Collection& gc) {
			return "GeometryCollection(geometries=" + std::to_string(gc.geometries.size()) + ")";
		});

	// Geometry
	pybind11::class_<Geometry>(m, "Geometry")
		.def(pybind11::init<>())
		.def_readwrite("Bbox",         &Geometry::bbox)
		.def("Is_Point",               &Geometry::Is_Point)
		.def("Is_Multi_Point",         &Geometry::Is_Multi_Point)
		.def("Is_Line_String",         &Geometry::Is_Line_String)
		.def("Is_Multi_Line_String",   &Geometry::Is_Multi_Line_String)
		.def("Is_Polygon",             &Geometry::Is_Polygon)
		.def("Is_Multi_Polygon",       &Geometry::Is_Multi_Polygon)
		.def("Is_Geometry_Collection", &Geometry::Is_Geometry_Collection)
		.def("Get_Point",               [](const Geometry& g) {return g.Get_Point(); })
		.def("Get_Multi_Point",         [](const Geometry& g) {return g.Get_Multi_Point(); })
		.def("Get_Line_String",         [](const Geometry& g) {return g.Is_Line_String(); })
		.def("Get_Multi_Line_String",   [](const Geometry& g) {return g.Get_Multi_Line_String(); })
		.def("Get_Polygon",             [](const Geometry& g) {return g.Get_Polygon(); })
		.def("Get_Multi_Polygon",       [](const Geometry& g) {return g.Get_Multi_Polygon(); })
		.def("Get_Geometry_Collection", [](const Geometry& g) {return g.Get_Geometry_Collection(); })
		.def("__repr__", [](const Geometry& g) {
			if (g.Is_Point())               return "Geometry(Point)";
			if (g.Is_Multi_Point())         return "Geometry(MultiPoint)";
			if (g.Is_Line_String())         return "Geometry(LineString)";
			if (g.Is_Multi_Line_String())   return "Geometry(MultiLineString)";
			if (g.Is_Polygon())             return "Geometry(Polygon)";
			if (g.Is_Multi_Polygon())       return "Geometry(MultiPolygon)";
			if (g.Is_Geometry_Collection()) return "Geometry(GeometryCollection)";
			return "Geometry(Unknown)";
		});

	// Feature
	pybind11::class_<Feature>(m, "Feature")
		.def(pybind11::init<>())
		.def_readwrite("geometry", &Feature::geometry)
		.def_readwrite("properties", &Feature::properties)
		.def_readwrite("id", &Feature::id)
		.def_readwrite("bbox", &Feature::bbox)
		.def("__repr__", [](const Feature& f) {
			return "Feature(id=" + (f.id ? *f.id : "none") + ", geometry=" + (f.geometry ? "present" : "none") + ")";
		});

	// FeatureCollection
	pybind11::class_<Feature_Collection>(m, "FeatureCollection")
		.def(pybind11::init<>())
		.def_readwrite("features", &Feature_Collection::features)
		.def_readwrite("bbox", &Feature_Collection::bbox)
		.def_readwrite("id", &Feature_Collection::id)
		.def("__repr__", [](const Feature_Collection& fc) {
			return "FeatureCollection(features=" + std::to_string(fc.features.size()) + ")";
		});

	// main container)
	pybind11::class_<Root>(m, "Root")
		.def(pybind11::init<>())
		.def("Is_Feature",            &Root::Is_Feature)
		.def("Is_Feature_Collection", &Root::Is_Feature_Collection)
		.def("Is_Geometry",           &Root::Is_Geometry)
		.def("Get_Feature",            [](const Root& r) {return r.Get_Feature();})
		.def("Get_Feature_Collection", [](const Root& r) {return r.Get_Feature_Collection();})
		.def("Get_Geometry",           [](const Root& r) {return r.Get_Geometry(); })
		.def("__repr__", [](const Root& g) {
			if (g.Is_Feature()) return "Root(Feature)";
			if (g.Is_Feature_Collection()) return "Root(FeatureCollection)";
			if (g.Is_Geometry()) return "Root(Geometry)";
			return "Root(Unknown)";
		});
}
