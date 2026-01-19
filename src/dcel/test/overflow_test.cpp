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
#include "simple_exemple.h"

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


TEST(DCEL, Overflow_Vertices)
{

	static O::Configuration::DCEL config{
		1,
		1000,
		1000,
		1e-9,
		O::Configuration::DCEL::Merge_Strategy::AT_FIRST
	};

	Auto_Builder auto_builder(config);
	rapidjson::StringStream ss(Simple_Exemple::json.c_str());
	rapidjson::Reader reader;

	try {
		reader.Parse(ss, auto_builder);
	}
	catch(O::DCEL::Exception& ex)
	{
		ASSERT_EQ(ex.type, O::DCEL::Exception::VERTICES_OVERFLOW);
	}
}

TEST(DCEL, Overflow_Half_Edge)
{

	static O::Configuration::DCEL config{
		100,
		1,
		1000,
		1e-9,
		O::Configuration::DCEL::Merge_Strategy::AT_FIRST
	};

	Auto_Builder auto_builder(config);
	rapidjson::StringStream ss(Simple_Exemple::json.c_str());
	rapidjson::Reader reader;

	try {
		reader.Parse(ss, auto_builder);
	}
	catch(O::DCEL::Exception& ex)
	{
		ASSERT_EQ(ex.type, O::DCEL::Exception::HALF_EDGES_OVERFLOW);
	}
}

TEST(DCEL, Overflow_Face)
{

	static O::Configuration::DCEL config{
		100,
		1000,
		1,
		1e-9,
		O::Configuration::DCEL::Merge_Strategy::AT_FIRST
	};

	Auto_Builder auto_builder(config);
	rapidjson::StringStream ss(Simple_Exemple::json.c_str());
	rapidjson::Reader reader;

	try {
		reader.Parse(ss, auto_builder);
	}
	catch(O::DCEL::Exception& ex)
	{
		ASSERT_EQ(ex.type, O::DCEL::Exception::FACES_OVERFLOW);
	}
}