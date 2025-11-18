#ifndef SRC_DCEL_TEST_DECEL_TEST_TEST_H
#define SRC_DCEL_TEST_DECEL_TEST_TEST_H

#include "include/dcel/builder.h"            // DCEL::Builder
#include "include/dcel/dcel.h"               // DCEL::DCEL, types
#include "include/dcel/feature_info.h"       // DCEL::FeatureInfo (see earlier design)
#include "include/dcel/exporter.h"    // DCEL::GeoJSON_Exporter
#include "include/geojson/geojson.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/geometry_type/polygon.h"
#include "include/geojson/geometry_type/multi_polygon.h"
#include "include/geojson/position.h"
#include "include/geojson/properties.h"
#include "include/geojson/bbox.h"

bool Nearly_Equal(double a, double b, double eps = 1e-9);
bool Positions_Equal(const GeoJSON::Position& a, const GeoJSON::Position& b);
bool Ring_Equal(const std::vector<GeoJSON::Position>& r1, const std::vector<GeoJSON::Position>& r2);
bool Polygon_Equal(const GeoJSON::Polygon& p1, const GeoJSON::Polygon& p2);
bool Multi_Polygon_Equal(const GeoJSON::Multi_Polygon& m1, const GeoJSON::Multi_Polygon& m2);
bool Property_Equal(const GeoJSON::Property& a, const GeoJSON::Property& b);
void Assert_Features_Equal(const GeoJSON::Feature& expected, const GeoJSON::Feature& actual);
GeoJSON::Feature Make_Simple_Polygon_Feature(const std::string& id, const std::vector<std::vector<std::pair<double,double>>>& ringsCoords, const GeoJSON::Property& properties, std::optional<GeoJSON::Bbox> bbox = std::nullopt);
GeoJSON::Feature Make_Multi_Polygon_Feature(const std::string& id, const std::vector<std::vector<std::vector<std::pair<double,double>>>>& polygonsRings, const GeoJSON::Property& properties, std::optional<GeoJSON::Bbox> bbox = std::nullopt);

#endif //SRC_DCEL_TEST_DECEL_TEST_TEST_H