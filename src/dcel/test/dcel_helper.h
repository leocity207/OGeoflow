#ifndef SRC_DCEL_TEST_DECEL_TEST_TEST_H
#define SRC_DCEL_TEST_DECEL_TEST_TEST_H

#include "dcel/builder.h"            // DCEL::Builder
#include "dcel/storage.h"            // DCEL::Storage, types
#include "dcel/feature_info.h"       // DCEL::FeatureInfo (see earlier design)
#include "dcel/exporter.h"           // DCEL::GeoJSON_Exporter
#include "geojson/root.h"
#include "geojson/object/feature.h"
#include "geojson/geometry_type/polygon.h"
#include "geojson/geometry_type/multi_polygon.h"
#include "geojson/position.h"
#include "geojson/properties.h"
#include "geojson/bbox.h"

bool Nearly_Equal(double a, double b, double eps = 1e-9);
bool Positions_Equal(const O::GeoJSON::Position& a, const O::GeoJSON::Position& b);
bool Ring_Equal(const std::vector<O::GeoJSON::Position>& r1, const std::vector<O::GeoJSON::Position>& r2);
bool Polygon_Equal(const O::GeoJSON::Polygon& p1, const O::GeoJSON::Polygon& p2);
bool Multi_Polygon_Equal(const O::GeoJSON::Multi_Polygon& m1, const O::GeoJSON::Multi_Polygon& m2);
bool Property_Equal(const O::GeoJSON::Property& a, const O::GeoJSON::Property& b);
void Assert_Features_Equal(const O::GeoJSON::Feature& expected, const O::GeoJSON::Feature& actual);
O::GeoJSON::Feature Make_Simple_Polygon_Feature(const std::string& id, const std::vector<std::vector<std::pair<double,double>>>& ringsCoords, const O::GeoJSON::Property& properties, std::optional<O::GeoJSON::Bbox> bbox = std::nullopt);
O::GeoJSON::Feature Make_Multi_Polygon_Feature(const std::string& id, const std::vector<std::vector<std::vector<std::pair<double,double>>>>& polygonsRings, const O::GeoJSON::Property& properties, std::optional<O::GeoJSON::Bbox> bbox = std::nullopt);

#endif //SRC_DCEL_TEST_DECEL_TEST_TEST_H