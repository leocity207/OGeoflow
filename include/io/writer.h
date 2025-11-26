#ifndef IO_WRITER_H
#define IO_WRITER_H

#include "include/geojson/root.h"
#include "include/geojson/properties.h"
#include "include/geojson/position.h"
#include "include/geojson/bbox.h"
#include "include/geojson/object/geometry.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/object/feature_collection.h"

// RapidJSON headers (assumed in include path)
#include <rapidjson/writer.h>

namespace O::GeoJSON::IO 
{
	/**
	 * @brief this class can be used to write down ``O::GeoJSON`` object to an outstream.
	 * @tparam Out_Stream outputing stream type from ``rapidjson`` (ex: rapidjson::OStreamWrapper, rapidjson::FileWriteStream).
	 */
	template <class Out_Stream>
	class Writer : public rapidjson::Writer<Out_Stream>
	{
	public:
		/**
		 * @param out outsream outputing stream sink from ``rapidjson`` (ex: rapidjson::OStreamWrapper, rapidjson::FileWriteStream).
		 */
		Writer(Out_Stream& out);

		/// @name Write functions
		/// @brief Implementation for every type to sink a GeoJSON object to the Out_Stream. in practice only Write_GeoJSON_Object should be used.
		/// @{
		void Write_Position          (const O::GeoJSON::Position& p);
		void Write_Bbox              (const O::GeoJSON::Bbox& bbox);
		void Write_Property_Value    (const O::GeoJSON::Property& prop);
		void Write_Properties        (const O::GeoJSON::Property& props);
		void Write_Feature           (const O::GeoJSON::Feature& f);
		void Write_Geometry_Value    (const O::GeoJSON::Geometry& g);
		void Write_Feature_Collection(const O::GeoJSON::Feature_Collection& f);
		void Write_GeoJSON_Object    (const O::GeoJSON::Root& f);
		/// @}
	};
}
	
#endif // IO_WRITER_H
