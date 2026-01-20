#include <gtest/gtest.h>

// DCEL
#include "dcel_builder_test.h"
#include "dcel/builder.h"
#include "dcel/face.h"
#include "dcel/vertex.h"
#include "dcel/half_edge.h"
#include "dcel/exception.h"

// IO
#include "io/feature_parser.h"

// EXEMPLE
#include "proximity_exemple/simple_exemple.h"

// CONFIGURATION
#include "configuration/dcel.h"

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


TEST(DCEL, Proximity_Test)
{

	static O::Configuration::DCEL config{
		1000,
		1000,
		1000,
		0.1,
		O::Configuration::DCEL::Merge_Strategy::AT_FIRST
	};

	Auto_Builder auto_builder(config);
	rapidjson::StringStream ss(Simple_Proximity_Exemple::json.c_str());
	rapidjson::Reader reader;
	ASSERT_TRUE(reader.Parse(ss, auto_builder));
	auto opt_dcel = auto_builder.Get_Dcel();
	ASSERT_TRUE(opt_dcel.has_value());
	auto& dcel = opt_dcel.value();
	for (auto&& [vertex, expected_coord] : O::Zip(dcel.vertices, Simple_Proximity_Exemple::expected_coords))
	{
		SCOPED_TRACE(std::format("vertex: {} {} but expected: {} {}", vertex.x, vertex.y, expected_coord.first, expected_coord.second));
		EXPECT_EQ(vertex.x, expected_coord.first);
		EXPECT_EQ(vertex.y, expected_coord.second);
	}
}