#include "dcel/exporter.h"

// STL
#include <unordered_map>
#include <cassert>
#include <map>

// UTILS
#include <utils/zip.h>

// DCEL
#include "dcel/face.h"

using namespace O;

std::vector<GeoJSON::Position> DCEL::Exporter::To_GeoJSON::Extract_Ring(const Face& face)
{
	std::vector<GeoJSON::Position> coords;
	Half_Edge* e = face.edge;
	do {
		const Vertex& v = *e->tail;
		coords.emplace_back(v.x, v.y);
		e = e->next;
	} while (e != face.edge);

	if (!coords.empty() && ((coords.front().altitude != coords.back().altitude) || (coords.front().latitude != coords.back().latitude) || (coords.front().longitude != coords.back().longitude)))
		coords.push_back(coords.front());

	return coords;
}

std::vector<Unowned_Ptr<const DCEL::Face>> DCEL::Exporter::To_GeoJSON::Collect_Face_From_Feature_ID(const Storage& dcel, size_t feature_id)
{
	std::vector<Unowned_Ptr<const Face>> faces;
	for (auto& face : dcel.faces)
    	if (face.associated_feature_index == feature_id)
			faces.push_back(&face);
	return faces;
}

std::unordered_map<const DCEL::Face*, GeoJSON::Polygon> DCEL::Exporter::To_GeoJSON::Create_Polygones(const std::vector<Unowned_Ptr<const Face>>& faces)
{
	std::unordered_map<const DCEL::Face*, GeoJSON::Polygon> polygons;
	for (auto face : faces)
	{
		auto ring = Extract_Ring( *face);
		if (face->outer_face == nullptr)
		{
			if (polygons[face].rings.size())
			{
				GeoJSON::Polygon tmps = std::move(polygons[face]);
				polygons[face] = GeoJSON::Polygon{};
				polygons[face].rings.emplace_back(std::move(ring));
				for (auto&& tmp : tmps.rings)
					polygons[face].rings.emplace_back(std::move(tmp));
			}
			else
				polygons[face].rings.emplace_back(ring);
		}
		else
			polygons[face->outer_face].rings.emplace_back(ring);
	}
	assert(polygons.size());
	return polygons;
}

GeoJSON::Root DCEL::Exporter::To_GeoJSON::Convert(const Storage& dcel, const Feature_Info& info)
{
	GeoJSON::Feature_Collection featureCollection;

	for ( size_t i : std::views::iota(0ul, info.feature_properties.size()))
	{
		GeoJSON::Geometry geometry;
		auto faces = Collect_Face_From_Feature_ID(dcel, i);
		std::unordered_map<const DCEL::Face*, GeoJSON::Polygon> polygons = Create_Polygones(faces);

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