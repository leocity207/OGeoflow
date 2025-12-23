#include "dcel_exporter_test.h"

// DCEL
#include "dcel/exporter.h"
#include "dcel/builder.h"

// IO
#include "io/feature_parser.h"
#include "io/writer.h"

// EXEMPLE
#include "hole_exemple.h"
#include "multi_polygon_exemple.h"
#include "reverse_exemple.h"
#include "simple_exemple.h"

// RAPIDJSON
#include <rapidjson/ostreamwrapper.h>

// CONFIGURATION
#include "configuration/dcel.h"

static O::Configuration::DCEL config{
	1000,
	1000,
	1000,
	1e-9,
	O::Configuration::DCEL::Merge_Strategy::AT_FIRST
};

class Auto_Builder : public O::DCEL::Builder::From_GeoJSON, public O::GeoJSON::IO::Feature_Parser<Auto_Builder> {
public:
	using O::DCEL::Builder::From_GeoJSON::On_Full_Feature;
	using O::DCEL::Builder::From_GeoJSON::On_Root;
	Auto_Builder(const O::Configuration::DCEL& conf) :
		O::DCEL::Builder::From_GeoJSON(conf),
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

	Auto_Builder auto_builder(config);
	rapidjson::StringStream ss(TypeParam::json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());

	auto opt_feature = auto_builder.Get_Feature_Info();
	ASSERT_TRUE(opt_feature.has_value());
	auto& feature = opt_feature.value();
	auto geojson = O::DCEL::Exporter::To_GeoJSON::Convert(feature);
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