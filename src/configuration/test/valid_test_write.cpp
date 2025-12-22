#include <gtest/gtest.h>

#include "configuration/dcel_adapter.h"

#include <configuration/application/json_writer.h>

#include "locals.h"

using namespace O::Configuration;


TEST(Valid_Test_Write, Regular)
{
	DCEL dcel_conf
	{
		100,
		200,
		300,
		0.001,
		DCEL::Merge_Strategy::AT_FIRST
	};


	Application::Container<DCEL> container{ dcel_conf };

	auto serialized = Application::Write_As_JSON_String(container);

	EXPECT_EQ(serialized, R"({"DCEL":{"max_vertices":100,"max_half_edges":200,"max_faces":300,"position_tolerance":0.001,"vertex_merge_strategy":"AT_FIRST"}})");
}