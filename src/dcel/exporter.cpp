#include "dcel/exporter.h"
#include <unordered_map>
#include <cassert>
#include <map>

#include <utils/zip.h>

using namespace O;

std::vector<GeoJSON::Position> DCEL::Exporter::Extract_Ring(const Storage& dcel, size_t face_id)
{
	std::vector<GeoJSON::Position> coords;
	size_t e = dcel.faces[face_id].edge;
	do {
		const auto& v = dcel.vertices[dcel.half_edges[e].origin];
		coords.emplace_back(v.x, v.y);
		e = dcel.half_edges[e].next;
	} while (e != dcel.faces[face_id].edge && e != NO_IDX);

	if (!coords.empty() && ((coords.front().altitude != coords.back().altitude) || (coords.front().latitude != coords.back().latitude) || (coords.front().longitude != coords.back().longitude)))
		coords.push_back(coords.front());

	return coords;
}

std::vector<size_t> DCEL::Exporter::Collect_Face_From_Feature_ID(const Storage& dcel, size_t feature_id)
{
	std::vector<size_t> indices;
	for (auto&& [face, index] : O::Zip_Index(dcel.faces))
    	if (face.associated_feature == feature_id)
        	indices.push_back(index);
	return indices;
}

std::unordered_map<size_t, GeoJSON::Polygon> DCEL::Exporter::Create_Polygones(const Storage& dcel, const std::vector<size_t>& faces_id)
{
	std::unordered_map<size_t, GeoJSON::Polygon> polygons;
	for (auto face_id : faces_id)
	{
		auto ring = Extract_Ring(dcel, face_id);
		if (dcel.faces[face_id].outer_face == NO_IDX)
		{
			if (polygons[face_id].rings.size())
			{
				GeoJSON::Polygon tmps = std::move(polygons[face_id]);
				polygons[face_id] = GeoJSON::Polygon{};
				polygons[face_id].rings.emplace_back(std::move(ring));
				for (auto&& tmp : tmps.rings)
					polygons[face_id].rings.emplace_back(std::move(tmp));
			}
			else
				polygons[face_id].rings.emplace_back(ring);
		}
		else
			polygons[dcel.faces[face_id].outer_face].rings.emplace_back(ring);
	}
	assert(polygons.size());
	return polygons;
}

GeoJSON::Root DCEL::Exporter::Convert(const Storage& dcel, const Feature_Info& info)
{
	GeoJSON::Feature_Collection featureCollection;

	for ( size_t i : std::views::iota(0ul, info.feature_properties.size()))
	{
		GeoJSON::Geometry geometry;
		auto faces_id = Collect_Face_From_Feature_ID(dcel, i);
		std::unordered_map<size_t, GeoJSON::Polygon> polygons = Create_Polygones(dcel, faces_id);

		// Create the Geometry (Polygon or multiPolygon)
		if(polygons.size() == 1)
			geometry.value = std::move(polygons.begin()->second);
		else
		{
			GeoJSON::Multi_Polygon multipolygon;
			for(auto& polygon_kv : polygons)
				multipolygon.polygons.emplace_back(polygon_kv.second);
			geometry.value = std::move(multipolygon);
		}

		// Create the feature
		GeoJSON::Feature feature;
		feature.geometry = std::move(geometry);
		feature.properties = info.feature_properties[i];

		if (i < info.ids.size())
			feature.id = info.ids[i];
		if (i < info.bboxes.size())
			feature.bbox = info.bboxes[i];

		featureCollection.features.emplace_back(std::move(feature));
	}

	if (info.has_root)
	{
		featureCollection.id = info.root_id;
		featureCollection.bbox = info.root_bbox;
	}

	GeoJSON::Root result;
	result.object = std::move(featureCollection);
	return result;
}