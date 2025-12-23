#include <gtest/gtest.h>

#include "configuration/dcel_adapter.h"
#include <configuration/application/json_builder.h>

#include "locals.h"

using namespace O::Configuration;

TEST(Valid_Test_Build, Regular)
{
	auto expected_app_config = Application::Build_From_JSON_File<DCEL>(Local::ROOT_PATH() / "src/configuration/test/dcel.json");
	EXPECT_TRUE(expected_app_config.Has_Value());
	auto app_config = std::move(expected_app_config.Value());
	auto& dcel_conf = app_config.Get<DCEL>();

	EXPECT_EQ(dcel_conf.max_vertices, 1000);
	EXPECT_EQ(dcel_conf.max_half_edges, 100000);
	EXPECT_EQ(dcel_conf.max_faces, 50);
	EXPECT_EQ(dcel_conf.position_tolerance, 1e-8);
	EXPECT_EQ(dcel_conf.vertex_merge_strategy, DCEL::Merge_Strategy::AT_FIRST);
}