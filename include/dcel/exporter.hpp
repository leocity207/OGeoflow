#ifndef DCEL_EXPORTER_H
#define DCEL_EXPORTER_H

#include "dcel/exporter.h"

// STL
#include <unordered_map>
#include <cassert>
#include <map>

// UTILS
#include <utils/zip.h>

// DCEL
#include "dcel/face.h"

template<class Vertex, class Half_Edge, class Face>
std::vector<O::GeoJSON::Position> O::DCEL::Exporter::To_GeoJSON<Vertex, Half_Edge, Face>::Extract_Ring(const Face& face)
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

template<class Vertex, class Half_Edge, class Face>
O::GeoJSON::Polygon O::DCEL::Exporter::To_GeoJSON<Vertex, Half_Edge, Face>::Create_Polygones(const std::vector<Unowned_Ptr<Face>>& faces)
{
	GeoJSON::Polygon polygons;
	for (auto face : faces)
	{
		auto ring = Extract_Ring( *face);
		polygons.rings.emplace_back(ring);
	}
	assert(polygons.rings.size());
	return polygons;
}

template<class Vertex, class Half_Edge, class Face>
O::GeoJSON::Root O::DCEL::Exporter::To_GeoJSON<Vertex, Half_Edge, Face>::Convert(const O::DCEL::Feature_Info<Face>& info)
{
	GeoJSON::Feature_Collection featureCollection;

	for ( auto&& [ polygons_faces, i] : O::Zip_Index(info.faces))
	{
		GeoJSON::Geometry geometry;

		// Create the Geometry (Polygon or multiPolygon)
		if(polygons_faces.size() == 1)
			geometry.value = Create_Polygones(polygons_faces.front());
		else
		{
			GeoJSON::Multi_Polygon multipolygon;
			for(auto& polygon_faces : polygons_faces)
				multipolygon.polygons.emplace_back(std::move(Create_Polygones(polygon_faces).rings));
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

#endif //DCEL_EXPORTER_H