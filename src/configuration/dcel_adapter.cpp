#include "configuration/dcel_adapter.h"

//rapidjson
#include <rapidjson/filewritestream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace O::Configuration;

std::optional<Error> DCEL_Adapter::Load_From_JSON(const rapidjson::Value& v)
{
	if(v.HasMember("max_vertices"))
	{
		if(!v["max_vertices"].IsUint64())
			return Error::MAX_SHOULD_BE_UNSIGNED_VALUE;
		data.max_vertices = v["max_vertices"].GetUint64();
	}
	if(v.HasMember("max_half_edges"))
	{
		if(!v["max_half_edges"].IsUint64())
			return Error::MAX_SHOULD_BE_UNSIGNED_VALUE;
		data.max_half_edges = v["max_half_edges"].GetUint64();
	}
	if(v.HasMember("max_faces"))
	{
		if(!v["max_faces"].IsUint64())
			return Error::MAX_SHOULD_BE_UNSIGNED_VALUE;
		data.max_faces = v["max_faces"].GetUint64();
	}
	if(v.HasMember("position_tolerance"))
	{
		if(!v["position_tolerance"].IsDouble())
			return Error::POINT_PRECISION_SHOULD_BE_DOUBLE;
		data.position_tolerance = v["position_tolerance"].GetDouble();
	}
	if(v.HasMember("vertex_merge_strategy"))
	{
		if(!v["vertex_merge_strategy"].IsString())
			return Error::MERGE_STRATEGY_SHOULD_BE_STRING;
		if(std::string_view(v["vertex_merge_strategy"].GetString()) == "AT_FIRST")
		{
			data.vertex_merge_strategy = DCEL::Merge_Strategy::AT_FIRST;
		}
		else
			return Error::MERGE_STRATEGY_UNKNOWN;
	}
	return std::nullopt;
}

template<class RapidJSON_Writer>
void DCEL_Adapter::To_JSON(RapidJSON_Writer& writer, const DCEL& data) const
{
	writer.StartObject();
	writer.Key("max_vertices");
	writer.Uint64(data.max_vertices);
	writer.Key("max_half_edges");
	writer.Uint64(data.max_half_edges);
	writer.Key("max_faces");
	writer.Uint64(data.max_faces);
	writer.Key("position_tolerance");
	writer.Double(data.position_tolerance);
	writer.Key("vertex_merge_strategy");
	switch (data.vertex_merge_strategy)
	{
		case DCEL::Merge_Strategy::AT_FIRST:
			writer.String("AT_FIRST");
			break;
	}
	writer.EndObject();
}


template void DCEL_Adapter::To_JSON<rapidjson::Writer<rapidjson::FileWriteStream>>(rapidjson::Writer<rapidjson::FileWriteStream>& writer,const DCEL& data) const;
template void DCEL_Adapter::To_JSON<rapidjson::Writer<rapidjson::StringBuffer>>(rapidjson::Writer<rapidjson::StringBuffer>& writer, const DCEL& data) const;
