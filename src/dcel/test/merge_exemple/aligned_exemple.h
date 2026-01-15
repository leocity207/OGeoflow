#ifndef SRC_DCEL_TEST_MERGE_EXEMPLE_ALINGED_EXEMPLE_H
#define SRC_DCEL_TEST_MERGE_EXEMPLE_ALINGED_EXEMPLE_H

#include "common.h"

struct Aligned_Exemple
{
	static inline const std::vector<Merge_Exemple::Vertex> vertices =
	{
		{0,0,{0,2}},
		{4,0,{1}},
		{-4,0,{3}},
	};

	static inline const std::vector<Merge_Exemple::Half_Edge> edges =
	{
		{ 0,1,1,3,1,0},
		{ 1,0,0,0,2,1},
		{ 0,2,3,1,3,1},
		{ 2,0,2,2,0,0},
	};

	static inline const std::vector<Merge_Exemple::Move> moves =
	{
		{4,0,0},
	};

	static inline const std::vector<Merge_Exemple::Vertex> expected_vertices =
	{
		{-4,0,{0}},
		{4,0,{1}},
	};

	static inline const std::vector<Merge_Exemple::Half_Edge> expected_edges =
	{
		{ 0,1,1,1,1,0},
		{ 1,0,0,0,0,0},
	};
};

#endif //SRC_DCEL_TEST_MERGE_EXEMPLE_ALINGED_EXEMPLE_H