#include "dcel_test.h"

#include "dcel_helper.h"

TEST(DCEL_Builder_Exporter, SimplePolygonRoundTrip) {
    O::DCEL::Builder builder;

    // create simple square polygon
    O::GeoJSON::Property props;
    props = O::GeoJSON::Property::Object{};
    props = std::move(O::GeoJSON::Property::Object{{{"name", O::GeoJSON::Property("square")}}});
    // simpler: just assign string property
    props = std::string("square-property");

    std::vector<std::vector<std::pair<double,double>>> rings = {
        { {0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0} } // exterior (will be closed by helper)
    };

    auto f = Make_Simple_Polygon_Feature("feature-1", rings, props);

    // feed to builder
    EXPECT_TRUE(builder.On_Full_Feature(std::move(f)));

    // finalize
    auto dcelOpt = builder.Get_Dcel();
    ASSERT_TRUE(dcelOpt.has_value());
    O::DCEL::Storage dcel = std::move(*dcelOpt);

    auto infoOpt = builder.Get_Feature_Info();
    ASSERT_TRUE(infoOpt.has_value());
    O::DCEL::Feature_Info info = *infoOpt;

    // basic DCEL checks
    EXPECT_GT(dcel.vertices.size(), (size_t)0);
    EXPECT_GT(dcel.halfedges.size(), (size_t)0);
    EXPECT_GT(dcel.faces.size(), (size_t)0);

    // convert back to GeoJSON
    O::GeoJSON::Root out = O::DCEL::Exporter::Convert(dcel, info);

    // compare: one feature expected
    ASSERT_TRUE(out.Is_Feature_Collection());
    const auto& fc = out.Get_Feature_Collection();
    ASSERT_EQ(fc.features.size(), (size_t)1);

    // Build expected feature again to compare
    O::GeoJSON::Feature expected = Make_Simple_Polygon_Feature("feature-1", rings, props);
    Assert_Features_Equal(expected, fc.features[0]);
}

TEST(DCEL_Builder_Exporter, PolygonWithHoleRoundTrip) {
    O::DCEL::Builder builder;

    O::GeoJSON::Property props = std::string("poly-with-hole");

    // square with a small square hole
    std::vector<std::vector<std::pair<double,double>>> rings = {
        { {0.0,0.0}, {0.0,5.0}, {5.0,5.0}, {5.0,0.0} }, // exterior
        { {1.0,1.0}, {1.0,2.0}, {2.0,2.0}, {2.0,1.0} }  // hole
    };

    auto f = Make_Simple_Polygon_Feature("feature-hole", rings, props);

    EXPECT_TRUE(builder.On_Full_Feature(std::move(f)));

    auto dcelOpt = builder.Get_Dcel();
    ASSERT_TRUE(dcelOpt.has_value());
    O::DCEL::Storage dcel = std::move(*dcelOpt);

    auto infoOpt = builder.Get_Feature_Info();
    ASSERT_TRUE(infoOpt.has_value());
    O::DCEL::Feature_Info info = *infoOpt;

    EXPECT_GT(dcel.faces.size(), (size_t)0);

    O::GeoJSON::Root out = O::DCEL::Exporter::Convert(dcel, info);
    ASSERT_TRUE(out.Is_Feature_Collection());
    const auto& fc = out.Get_Feature_Collection();
    ASSERT_EQ(fc.features.size(), (size_t)1);

    O::GeoJSON::Feature expected = Make_Simple_Polygon_Feature("feature-hole", rings, props);
    Assert_Features_Equal(expected, fc.features[0]);
}

TEST(DCEL_Builder_Exporter, MultiPolygonRoundTrip) {
    O::DCEL::Builder builder;

    O::GeoJSON::Property props = std::string("multipoly");

    // MultiPolygon: two disjoint squares
    std::vector<std::vector<std::vector<std::pair<double,double>>>> mp = {
        {
            { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} } // polygon 1 exterior
        },
        {
            { {10.0,10.0}, {10.0,11.0}, {11.0,11.0}, {11.0,10.0} } // polygon 2 exterior
        }
    };

    auto f = Make_Multi_Polygon_Feature("multi-1", mp, props);

    EXPECT_TRUE(builder.On_Full_Feature(std::move(f)));

    auto dcelOpt = builder.Get_Dcel();
    ASSERT_TRUE(dcelOpt.has_value());
    O::DCEL::Storage dcel = std::move(*dcelOpt);

    auto infoOpt = builder.Get_Feature_Info();
    ASSERT_TRUE(infoOpt.has_value());
    O::DCEL::Feature_Info info = *infoOpt;

    EXPECT_GT(dcel.vertices.size(), (size_t)0);

    O::GeoJSON::Root out = O::DCEL::Exporter::Convert(dcel, info);
    ASSERT_TRUE(out.Is_Feature_Collection());
    const auto& fc = out.Get_Feature_Collection();
    ASSERT_EQ(fc.features.size(), (size_t)1);

    // expected
    O::GeoJSON::Feature expected = Make_Multi_Polygon_Feature("multi-1", mp, props);
    Assert_Features_Equal(expected, fc.features[0]);
}

TEST(DCEL_Builder_Exporter, FeaturePropertiesAndBboxAndIdRoundTrip) {
    O::DCEL::Builder builder;

    // properties as an object
    O::GeoJSON::Property props;
    O::GeoJSON::Property::Object obj;
    obj["type"] = std::string("test");
    obj["value"] = 42;
    props = std::move(obj);

    O::GeoJSON::Bbox bbox;
    bbox.coordinates = std::array<double,4>{-1.0, -1.0, 2.0, 2.0};

    std::vector<std::vector<std::pair<double,double>>> rings = {
        { {0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0} }
    };

    O::GeoJSON::Feature f = Make_Simple_Polygon_Feature("id-123", rings, props, bbox);

    EXPECT_TRUE(builder.On_Full_Feature(std::move(f)));

    auto dcelOpt = builder.Get_Dcel();
    ASSERT_TRUE(dcelOpt.has_value());
    O::DCEL::Storage dcel = std::move(*dcelOpt);

    auto infoOpt = builder.Get_Feature_Info();
    ASSERT_TRUE(infoOpt.has_value());
    O::DCEL::Feature_Info info = *infoOpt;

    O::GeoJSON::Root out = O::DCEL::Exporter::Convert(dcel, info);
    ASSERT_TRUE(out.Is_Feature_Collection());
    const auto& fc = out.Get_Feature_Collection();
    ASSERT_EQ(fc.features.size(), (size_t)1);

    const auto& got = fc.features[0];
    // compare id
    ASSERT_TRUE(got.id.has_value());
    ASSERT_EQ(*got.id, "id-123");
    // compare bbox
    ASSERT_TRUE(got.bbox.has_value());
    ASSERT_EQ(got.bbox->Has_Altitude(), false);
    const auto be = got.bbox->Get();
    ASSERT_EQ(be[0], -1.0);
    ASSERT_EQ(be[2], 2.0);
    // compare properties (object)
    ASSERT_TRUE(got.properties.Is_Object());
    ASSERT_EQ(got.properties.Size(), 2u);
}