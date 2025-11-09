#ifndef IO_WRITER_H
#define IO_WRITER_H

#include "include/geojson/geojson.h"
#include "include/geojson/properties.h"
#include "include/geojson/position.h"
#include "include/geojson/bbox.h"
#include "include/geojson/object/geometry.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/object/feature_collection.h"

// RapidJSON headers (assumed in include path)
#include <rapidjson/writer.h>

namespace GeoJSON::IO 
{
	template <class Out_Stream>
	class Writer : public rapidjson::Writer<Out_Stream>
	{
	public:
		Writer(Out_Stream& out);

		void Write_Position(const Position& p);
		void Write_Bbox(const Bbox& bbox);
		void Write_Property_Value(const Property& prop);
		void Write_Properties(const Property& props);
		void Write_Feature(const Feature& f);
		void Write_Geometry_Value(const Geometry& g);
		void Write_Feature_Collection(const Feature_Collection& f);
		void Write_GeoJSON_Object(const GeoJSON& f);
	};
}
	
#endif // IO_WRITER_H
