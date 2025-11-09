// test_writer.cpp
#include <gtest/gtest.h>

#include "writer_test.h"

#include <sstream>
#include <string>

#include <rapidjson/ostreamwrapper.h>

#include "include/io/writer.h"
#include "include/geojson/geojson.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/object/feature_collection.h"
#include "include/geojson/object/geometry.h"
#include "include/geojson/position.h"
#include "include/geojson/properties.h"

using namespace GeoJSON;

static std::string SerializeToString(const ::GeoJSON::GeoJSON& obj)
{
	std::stringstream ss;
	rapidjson::OStreamWrapper osw(ss);
	::GeoJSON::IO::Writer<rapidjson::OStreamWrapper> writer(osw);
	writer.Write_GeoJSON_Object(obj);
	return ss.str();
}

TEST(Writer_Test, FeaturePointSimple)
{
	// Build Feature with Point + properties + id
	Feature feat;
	feat.geometry = Geometry{};
	// set geometry value to Point
	Point p;
	p.position.longitude = 2.2945;
	p.position.latitude  = 48.8584;
	feat.geometry->value = p;

	// properties: { "name": "Eiffel Tower" }
	Property::Object props_obj;
	props_obj["name"] = Property(std::string("Eiffel Tower"));
	feat.properties = Property(std::move(props_obj));

	feat.id = std::string("eiffel-1");

	::GeoJSON::GeoJSON root;
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
	Feature feat;
	feat.geometry = Geometry{};
	Point p;
	p.position.longitude = -0.1276;
	p.position.latitude  = 51.5074;
	feat.geometry->value = p;

	Property::Object props_obj;
	props_obj["city"] = Property(std::string("London"));
	feat.properties = Property(std::move(props_obj));
	feat.id = std::string("london-1");

	Feature_Collection fc;
	fc.features.push_back(feat);

	::GeoJSON::GeoJSON root;
	root.object = fc;

	const std::string out = SerializeToString(root);

	// Serializer writes FeatureCollection as: type, (maybe bbox), id, features
	// Our Feature inside is serialized as in the other test.
	const std::string expected =
		R"({"type":"FeatureCollection","features":[{"type":"Feature","id":"london-1","geometry":{"type":"Point","coordinates":[-0.1276,51.5074]},"properties":{"city":"London"}}]})";

	ASSERT_EQ(out, expected);
}