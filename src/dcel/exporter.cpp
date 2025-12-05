#include "dcel/exporter.h"
#include <unordered_map>
#include <cassert>


using namespace O;

std::vector<GeoJSON::Position> DCEL::Exporter::Extract_Ring(const Storage& dcel, size_t start_edge)
{
	std::vector<GeoJSON::Position> coords;
	size_t e = start_edge;
	do {
		const auto& v = dcel.vertices[dcel.half_edges[e].origin];
		coords.emplace_back(v.x, v.y);
		e = dcel.half_edges[e].next;
	} while (e != start_edge && e != NO_IDX);

	if (!coords.empty() && ((coords.front().altitude != coords.back().altitude) || (coords.front().latitude != coords.back().latitude) || (coords.front().longitude != coords.back().longitude)))
		coords.push_back(coords.front());

	return coords;
}

GeoJSON::Polygon DCEL::Exporter::Build_Polygon_From_Face(const Storage& dcel, size_t face_idx)
{
	const auto& face = dcel.faces[face_idx];
	std::vector<std::vector<GeoJSON::Position>> rings;

	// Exterior ring
	if (face.edge != NO_IDX)
		rings.push_back(Extract_Ring(dcel, face.edge));

	// Holes
	for (size_t innerEdge : face.inner_edges)
		rings.push_back(Extract_Ring(dcel, innerEdge));

	GeoJSON::Polygon polygon;
	polygon.rings = std::move(rings);
	return polygon;
}

std::vector<GeoJSON::Polygon> DCEL::Exporter::Collect_Polygons_For_Feature(const Storage& dcel, size_t featureIdx)
{
	std::vector<GeoJSON::Polygon> polygons;

	for (size_t f = 0; f < dcel.faces.size(); ++f)
	{
		const auto& face = dcel.faces[f];
		// Suppose the builder filled some mapping from featureIdx -> faces (or you can deduce it)
		if (face.associated_feature == NO_IDX) continue;

		// skip holes themselves, we only export outer faces
		if (face.outer_face == NO_IDX)
			polygons.push_back(Build_Polygon_From_Face(dcel, f));
	}

	return polygons;
}

GeoJSON::Root DCEL::Exporter::Convert(const Storage& dcel, const Feature_Info& info)
{
	GeoJSON::Feature_Collection featureCollection;

	for (size_t i = 0; i < info.feature_properties.size(); ++i)
	{
		auto polygons = Collect_Polygons_For_Feature(dcel, i);

		if (polygons.empty()) continue;

		GeoJSON::Geometry geometry;
		if (polygons.size() == 1)
			geometry.value = std::move(polygons[0]);
		else
		{
			GeoJSON::Multi_Polygon mp;
			mp.polygons = std::move(polygons);
			geometry.value = std::move(mp);
		}

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