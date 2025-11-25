// filter_test.cpp
#include <gtest/gtest.h>

#include "filter_test.h"

#include <rapidjson/reader.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/encodedstream.h>
#include <rapidjson/encodings.h>
//#include <rapidjson/strtod.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

#include <string>
#include <vector>
#include <optional>
#include <sstream>

#include "include/filter/feature.h"
#include "include/geojson/object/feature.h"
#include "include/geojson/object/feature_collection.h"
#include "include/geojson/object/geometry.h"
#include "include/geojson/properties.h"

using namespace O::GeoJSON;




// Predicate object that counts invocations and checks properties.name == "keep"
struct PredCountAndMatch
{
    int calls = 0;

    bool operator()(const Feature& f) noexcept
    {
        ++calls;
        if (!f.properties.Is_Object()) return false;
        const auto& obj = f.properties.Get_Object();
        auto it = obj.find("name");
        if (it == obj.end()) return false;
        if (!it->second.Is_String()) return false;
        return it->second.Get_String() == "keep";
    }
};

// Simple collector used as downstream: stores all received features and counts calls.
template<class Pred>
struct FeatureCollector : public Filter::Feature_Filter<FeatureCollector<Pred>, Pred >
{
    std::vector<Feature> features;
    std::optional<Bbox> root_bbox;
    std::optional<std::string> root_id;
    int root_calls = 0;

    bool On_Full_Feature(Feature&& f)
    {
        features.push_back(std::move(f));
        return true; // continue parsing
    }

    bool On_Root(std::optional<Bbox>&& bbox, std::optional<std::string>&& id)
    {
        ++root_calls;
        root_bbox = std::move(bbox);
        root_id = std::move(id);
        return true;
    }
};

static const char* kSampleFeatureCollection = R"(
{
  "type":"FeatureCollection",
  "features":[
    {
      "type":"Feature",
      "id":"f1",
      "geometry":{"type":"Point","coordinates":[0.0,0.0]},
      "properties":{"name":"keep","value":1}
    },
    {
      "type":"Feature",
      "id":"f2",
      "geometry":{"type":"Point","coordinates":[1.0,1.0]},
      "properties":{"name":"drop","value":2}
    },
    {
      "type":"Feature",
      "id":"f3",
      "geometry":{"type":"Point","coordinates":[2.0,2.0]},
      "properties":{"name":"keep","value":3}
    }
  ]
}
)";

// Test: ensure predicate is called for each feature and collector gets only kept features.
TEST(Feature_Filter_Test, FiltersAndForwardsCorrectly)
{
    // Prepare JSON input as RapidJSON StringStream
    rapidjson::StringStream ss(kSampleFeatureCollection);
    rapidjson::Reader reader;

    PredCountAndMatch pred;

    // Create filter: note Feature_Filter expects Downstream& and Pred
    FeatureCollector<PredCountAndMatch> filter(pred);

    // Parse: filter will receive SAX events and forward On_Full_Feature to collector when predicate true
    bool ok = reader.Parse(ss, filter);
    ASSERT_TRUE(ok) << "Parse failed: " << rapidjson::GetParseError_En(reader.GetParseErrorCode());

    // Predicate should be invoked exactly once per feature (3 features)
    EXPECT_EQ(filter.Get_Predicator().calls, 3);

    // Collector should have received only the two "keep" features (f1 and f3)
    ASSERT_EQ(filter.features.size(), 2u);
    EXPECT_TRUE(filter.features[0].id.has_value());
    EXPECT_TRUE(filter.features[1].id.has_value());
    EXPECT_EQ(filter.features[0].id.value(), "f1");
    EXPECT_EQ(filter.features[1].id.value(), "f3");

    // Verify the properties of the forwarded features are intact
    const auto& f1_props = filter.features[0].properties.Get_Object();
    ASSERT_NE(f1_props.find("value"), f1_props.end());
    EXPECT_TRUE(f1_props.at("value").Is_Integer());
    EXPECT_EQ(f1_props.at("value").Get_Int(), 1);

    const auto& f3_props = filter.features[1].properties.Get_Object();
    ASSERT_NE(f3_props.find("value"), f3_props.end());
    EXPECT_TRUE(f3_props.at("value").Is_Integer());
    EXPECT_EQ(f3_props.at("value").Get_Int(), 3);
}

// Additional test: if predicate keeps none, collector should be empty but predicate called per feature.
TEST(Feature_Filter_Test, DropsAllWhenPredicateFalse)
{
    rapidjson::StringStream ss(kSampleFeatureCollection);
    rapidjson::Reader reader;

    // predicate that always returns false but still counts
    struct PredAlwaysFalse { int calls = 0; bool operator()(const Feature&){ ++calls; return false; } } pred;

    FeatureCollector<PredAlwaysFalse> filter( pred);

    bool ok = reader.Parse(ss, filter);
    ASSERT_TRUE(ok);

    EXPECT_EQ(filter.Get_Predicator().calls, 3);          // predicate was called for each feature
    EXPECT_EQ(filter.features.size(), 0u); // nothing forwarded
}