#ifndef SRC_DCEL_TEST_MERGE_EXEMPLE_X_EXEMPLE_H
#define SRC_DCEL_TEST_MERGE_EXEMPLE_X_EXEMPLE_H

#include "common.h"

struct X_Exemple
{
	static inline const std::vector<Merge_Exemple::Vertex> vertex =
	{
		{0,0,{0,8,6}},
		{3,0,{2,1,4}},
		{5, 3,{3}},
		{5,-3,{5}},
		{-2-3,{7}},
		{-2,3,{9}},
	};

	static inline const std::vector<Merge_Exemple::Half_Edge> edges =
	{
		{ 0,1,1,9,2,0},
		{ 1,0,0,5,6,1},
		{ 1,2,3,0,3,0},
		{ 2,1,2,2,4,2},
		{ 1,3,5,3,5,2},
		{ 3,1,4,4,1,1},
		{ 0,4,5,1,7,1},
		{ 4,0,4,6,8,3},
		{ 0,5,5,7,9,3},
		{ 5,0,4,8,0,0},
	};

	static inline const std::vector<Merge_Exemple::Move> moves =
	{
		{0,0,1},
		{2,3,2},
		{2,-3,3}
	};

	static inline const std::vector<Merge_Exemple::Vertex> expected_vertex =
	{
		{0,0,{0,8,6}},
		{3,0,{2,1,4}},
		{5, 3,{3}},
		{5,-3,{5}},
		{-2-3,{7}},
	};

	static inline const std::vector<Merge_Exemple::Half_Edge> expected_edges =
	{
		{ 0,1,1,1,2,0},
		{ 1,0,0,3,0,1},
	};
};

#endif //SRC_DCEL_TEST_MERGE_EXEMPLE_X_EXEMPLE_H