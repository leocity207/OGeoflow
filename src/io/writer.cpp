#include "include/io/writer.h"

#include <cstdio>
#include <ostream>
#include <string>
#include <type_traits>


#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/filewritestream.h>

using namespace O::GeoJSON::IO;

template <class Out_Stream>
Writer<Out_Stream>::Writer(Out_Stream& out) :
	rapidjson::Writer<Out_Stream>(out)
{

}

template <class Out_Stream>
void Writer<Out_Stream>::Write_Position(const O::GeoJSON::Position& p)
{
	this->StartArray();
	this->Double(p.longitude);
	this->Double(p.latitude);
	if (p.altitude.has_value())
		this->Double(*p.altitude);
	this->EndArray();
}

template <class Out_Stream>
void Writer<Out_Stream>::Write_Bbox(const O::GeoJSON::Bbox& bbox)
{
	this->Key("bbox");
	this->StartArray();
	if (bbox.Has_Altitude())
	{
		const auto& arr = bbox.Get_With_Altitudes();
		for (double v : arr) this->Double(v);
	}
	else
	{
		const auto& arr = bbox.Get();
		for (double v : arr) this->Double(v);
	}
	this->EndArray();
}

template <class Out_Stream>
void Writer<Out_Stream>::Write_Property_Value(const O::GeoJSON::Property& prop)
{
	const auto& var = prop.m_value;
	std::visit([&](auto const& val) {
		using T = std::decay_t<decltype(val)>;
		if constexpr (std::is_same_v<T, std::monostate>)
			this->Null();
		else if constexpr (std::is_same_v<T, bool>)
			this->Bool(val);
		else if constexpr (std::is_same_v<T, std::int64_t>)
			this->Int64(val);
		else if constexpr (std::is_same_v<T, double>)
			this->Double(val);
		else if constexpr (std::is_same_v<T, std::string>)
			this->String(val.c_str(), static_cast<rapidjson::SizeType>(val.size()));
		else if constexpr (std::is_same_v<T, O::GeoJSON::Property::Array>)
		{
			this->StartArray();
			for (const auto& elem : val)
				Write_Property_Value(elem);
			this->EndArray();
		}
		else if constexpr (std::is_same_v<T, O::GeoJSON::Property::Object>)
		{
			this->StartObject();
			for (const auto& kv : val)
			{
				this->Key(kv.first.c_str(), static_cast<rapidjson::SizeType>(kv.first.size()));
				Write_Property_Value(kv.second);
			}
			this->EndObject();
		}
		else
		{
			// Unsupported variant type - emit null
			this->Null();
		}
	}, var);
}

template <class Out_Stream>
void Writer<Out_Stream>::Write_Properties(const O::GeoJSON::Property& props)
{
	// If properties is an object, write its fields; otherwise, write as null or primitive
	if (props.Is_Object())
	{
		const auto& obj = props.Get_Object();
		this->Key("properties");
		this->StartObject();
		for (const auto& kv : obj)
		{
			this->Key(kv.first.c_str(), static_cast<rapidjson::SizeType>(kv.first.size()));
			Write_Property_Value(kv.second);
		}
		this->EndObject();
	}
	else
	{
		// If it's not an object, still write something sensible:
		this->Key("properties");
		Write_Property_Value(props);
	}
}

template <class Out_Stream>
void Writer<Out_Stream>::Write_Feature(const O::GeoJSON::Feature& f)
{
	this->Key("type");
	this->String("Feature");
	if (f.bbox.has_value())
		Write_Bbox(*f.bbox);
	if (f.id.has_value())
	{
		this->Key("id");
		const std::string& s = *f.id;
		this->String(s.c_str(), static_cast<rapidjson::SizeType>(s.size()));
	}
	if (f.geometry.has_value())
	{
		this->Key("geometry");
		this->StartObject();
		Write_Geometry_Value(*f.geometry);
		this->EndObject();
	}
	else
	{
		this->Key("geometry");
		this->Null();
	}

	// properties
	Write_Properties(f.properties);
}

template <class Out_Stream>
void Writer<Out_Stream>::Write_Geometry_Value(const O::GeoJSON::Geometry& g)
{
	std::visit([&](auto const& geom) {
		using T = std::decay_t<decltype(geom)>;

		this->Key("type");
		if constexpr (std::is_same_v<T, O::GeoJSON::Point>)
			this->String("Point");
		else if constexpr (std::is_same_v<T, O::GeoJSON::Multi_Point>)
			this->String("MultiPoint");
		else if constexpr (std::is_same_v<T, O::GeoJSON::Line_String>)
			this->String("LineString");
		else if constexpr (std::is_same_v<T, O::GeoJSON::Multi_Line_String>)
			this->String("MultiLineString");
		else if constexpr (std::is_same_v<T, O::GeoJSON::Polygon>)
			this->String("Polygon");
		else if constexpr (std::is_same_v<T, O::GeoJSON::Multi_Polygon>)
			this->String("MultiPolygon");
		else if constexpr (std::is_same_v<T, O::GeoJSON::Geometry_Collection>)
			this->String("GeometryCollection");
		else
			this->String("Unknown");

		if (g.bbox.has_value())
			Write_Bbox(*g.bbox);

		if constexpr (std::is_same_v<T, O::GeoJSON::Point>)
		{
			this->Key("coordinates");
			Write_Position(geom.position);
		}
		else if constexpr (std::is_same_v<T, O::GeoJSON::Multi_Point>)
		{
			this->Key("coordinates");
			this->StartArray();
			for (auto const& p : geom.points) Write_Position(p);
			this->EndArray();
		}
		else if constexpr (std::is_same_v<T, O::GeoJSON::Line_String>)
		{
			this->Key("coordinates");
			this->StartArray();
			for (auto const& p : geom.positions) Write_Position(p);
			this->EndArray();
		}
		else if constexpr (std::is_same_v<T, O::GeoJSON::Multi_Line_String>)
		{
			this->Key("coordinates");
			this->StartArray();
			for (auto const& ls : geom.line_strings)
			{
				this->StartArray();
				for (auto const& p : ls.positions) Write_Position(p);
				this->EndArray();
			}
			this->EndArray();
		}
		else if constexpr (std::is_same_v<T, O::GeoJSON::Polygon>)
		{
			this->Key("coordinates");
			this->StartArray();
			for (auto const& ring : geom.rings)
			{
				this->StartArray();
				for (auto const& p : ring) Write_Position(p);
				this->EndArray();
			}
			this->EndArray();
		}
		else if constexpr (std::is_same_v<T, O::GeoJSON::Multi_Polygon>)
		{
			this->Key("coordinates");
			this->StartArray();
			for (auto const& poly : geom.polygons)
			{
				this->StartArray();
				for (auto const& ring : poly.rings)
				{
					this->StartArray();
					for (auto const& p : ring) Write_Position(p);
					this->EndArray();
				}
				this->EndArray();
			}
			this->EndArray();
		}
		else if constexpr (std::is_same_v<T, O::GeoJSON::Geometry_Collection>)
		{
			this->Key("geometries");
			this->StartArray();
			for (const auto& sp : geom.geometries)
			{
				this->StartObject();
				Write_Geometry_Value(*sp);
				this->EndObject();
			}
			this->EndArray();
		}
		else
		{
			this->Key("coordinates");
			this->Null();
		}

	}, g.value);
}

template <class Out_Stream>
void Writer<Out_Stream>::Write_Feature_Collection(const O::GeoJSON::Feature_Collection& fc)
{
	this->Key("type");
	this->String("FeatureCollection");
	if (fc.bbox.has_value())
		Write_Bbox(*fc.bbox);
	if (fc.id.has_value())
	{
		this->Key("id");
		const std::string& s = *fc.id;
		this->String(s.c_str(), static_cast<rapidjson::SizeType>(s.size()));
	}

	this->Key("features");
	this->StartArray();
	for (const auto& f : fc.features)
	{
		this->StartObject();
		Write_Feature(f);
		this->EndObject();
	}
	this->EndArray();
}

template <class Out_Stream>
void Writer<Out_Stream>::Write_GeoJSON_Object(const O::GeoJSON::Root& g)
{
	this->StartObject();
	if (g.Is_Feature())
		Write_Feature(g.Get_Feature());
	else if (g.Is_Feature_Collection())
		Write_Feature_Collection(g.Get_Feature_Collection());
	else if (g.Is_Geometry())
	{
		this->Key("type");
		// geometry as top-level geometry object
		// write type + coordinates or geometries + optional bbox
		const Geometry& geom = g.Get_Geometry();
		Write_Geometry_Value(geom);
	}
	this->EndObject();
}


template class Writer<rapidjson::OStreamWrapper>;
template class Writer<rapidjson::FileWriteStream>;