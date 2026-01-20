#include "dcel_exporter_test.h"

// DCEL
#include "dcel/exporter.h"
#include "dcel/builder.h"
#include "dcel/face.h"
#include "dcel/vertex.h"
#include "dcel/half_edge.h"

// IO
#include "io/feature_parser.h"
#include "io/writer.h"

// EXEMPLE
#include "dcel_exemple/hole_exemple.h"
#include "dcel_exemple/multi_polygon_exemple.h"
#include "dcel_exemple/reverse_exemple.h"
#include "dcel_exemple/simple_exemple.h"

// RAPIDJSON
#include <rapidjson/ostreamwrapper.h>

// CONFIGURATION
#include "configuration/dcel.h"

static O::Configuration::DCEL g_config{
	1000,
	1000,
	1000,
	1e-9,
	O::Configuration::DCEL::Merge_Strategy::AT_FIRST
};

struct Half_Edge_Impl : public O::DCEL::Half_Edge<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>> {
	using O::DCEL::Half_Edge<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>>::Half_Edge;
};

using Builder_From_GeoJSON = O::DCEL::Builder::From_GeoJSON<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>>;

class Auto_Builder : public Builder_From_GeoJSON, public O::GeoJSON::IO::Feature_Parser<Auto_Builder> {
public:
	using Builder_From_GeoJSON::On_Full_Feature;
	using Builder_From_GeoJSON::On_Root;
	Auto_Builder(const O::Configuration::DCEL& conf) :
		Builder_From_GeoJSON(conf),
		O::GeoJSON::IO::Feature_Parser<Auto_Builder>()
	{

	}
};

TYPED_TEST_SUITE_P(DCEL_Builder_Exporter);

static std::string SerializeToString(const O::GeoJSON::Root& obj)
{
	std::stringstream ss;
	rapidjson::OStreamWrapper osw(ss);
	O::GeoJSON::IO::Writer<rapidjson::OStreamWrapper> writer(osw);
	writer.Write_GeoJSON_Object(obj);
	return ss.str();
}

TYPED_TEST_P(DCEL_Builder_Exporter, Exporter)
{

	Auto_Builder auto_builder(g_config);
	rapidjson::StringStream ss(TypeParam::json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());

	auto opt_feature = auto_builder.Get_Feature_Info();
	ASSERT_TRUE(opt_feature.has_value());
	auto& feature = opt_feature.value();
	auto geojson = O::DCEL::Exporter::To_GeoJSON<O::DCEL::Vertex<Half_Edge_Impl>, Half_Edge_Impl, O::DCEL::Face<Half_Edge_Impl>>::Convert(feature);
	auto str = SerializeToString(geojson);
	EXPECT_EQ(str, TypeParam::expected_write);
}

REGISTER_TYPED_TEST_SUITE_P(
    DCEL_Builder_Exporter,
	Exporter
);

// Instantiate for all ts
using All_Test_Sets = ::testing::Types<
	Simple_Exemple,
	Reverse_Exemple,
	Hole_Exemple,
	Multi_Polygon_Exemple
>;

INSTANTIATE_TYPED_TEST_SUITE_P(DCEL, DCEL_Builder_Exporter, All_Test_Sets);