// test_writer.cpp
#include <gtest/gtest.h>

#include "writer_test.h"

#include <sstream>
#include <string>

#include <rapidjson/ostreamwrapper.h>

#include "include/io/writer.h"
#include "include/geojson/root.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/object/feature_collection.h"
#include "include/geojson/object/geometry.h"
#include "include/geojson/position.h"
#include "include/geojson/properties.h"


static std::string SerializeToString(const O::GeoJSON::Root& obj)
{
	std::stringstream ss;
	rapidjson::OStreamWrapper osw(ss);
	O::GeoJSON::IO::Writer<rapidjson::OStreamWrapper> writer(osw);
	writer.Write_GeoJSON_Object(obj);
	return ss.str();
}

TEST(Writer_Test, FeaturePointSimple)
{
	// Build Feature with Point + properties + id
	O::GeoJSON::Feature feat;
	feat.geometry = O::GeoJSON::Geometry{};
	// set geometry value to Point
	O::GeoJSON::Point p;
	p.position.longitude = 2.2945;
	p.position.latitude  = 48.8584;
	feat.geometry->value = p;

	// properties: { "name": "Eiffel Tower" }
	O::GeoJSON::Property::Object props_obj;
	props_obj["name"] = O::GeoJSON::Property(std::string("Eiffel Tower"));
	feat.properties = O::GeoJSON::Property(std::move(props_obj));

	feat.id = std::string("eiffel-1");

	O::GeoJSON::Root root;
	root.object = feat;

	const std::string out = SerializeToString(root);

	// Expect exact compact JSON (same order as the serializer: type, id, geometry, properties)
	const std::string expected = 
		R"({"type":"Feature","id":"eiffel-1","geometry":{"type":"Point","coordinates":[2.2945,48.8584]},"properties":{"name":"Eiffel Tower"}})";

	ASSERT_EQ(out, expected);
}

TEST(Writer_Test, FeatureCollectionSingleFeature)
{
	// Build a FeatureCollection with a single feature (same as previous)
	O::GeoJSON::Feature feat;
	feat.geometry = O::GeoJSON::Geometry{};
	O::GeoJSON::Point p;
	p.position.longitude = -0.1276;
	p.position.latitude  = 51.5074;
	feat.geometry->value = p;

	O::GeoJSON::Property::Object props_obj;
	props_obj["city"] = O::GeoJSON::Property(std::string("London"));
	feat.properties = O::GeoJSON::Property(std::move(props_obj));
	feat.id = std::string("london-1");

	O::GeoJSON::Feature_Collection fc;
	fc.features.push_back(feat);

	O::GeoJSON::Root root;
	root.object = fc;

	const std::string out = SerializeToString(root);

	// Serializer writes FeatureCollection as: type, (maybe bbox), id, features
	// Our Feature inside is serialized as in the other test.
	const std::string expected =
		R"({"type":"FeatureCollection","features":[{"type":"Feature","id":"london-1","geometry":{"type":"Point","coordinates":[-0.1276,51.5074]},"properties":{"city":"London"}}]})";

	ASSERT_EQ(out, expected);
}