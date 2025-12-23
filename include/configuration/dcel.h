#ifndef CONFIGURATION_DCEL_H
#define CONFIGURATION_DCEL_H

// STL
#include <variant>
#include <vector>

namespace O::Configuration
{
	
	
	struct DCEL
	{
		enum class Merge_Strategy
		{
			//AT_CENTER, //not implemented
			AT_FIRST,
			//AT_LAST // not implemented
			//AT_TOLERENCE // not implemented
		};
		size_t max_vertices = 0ul;              ///< maximum number of vertices to prealocate
		size_t max_half_edges = 0ul;            ///< maximum number of half edges to prealocate
		size_t max_faces = 0ul;                 ///< maximum number of faces that will be prealicated
		double position_tolerance = 1e-9;       ///< this is a ceil to tell if two point are equal when inserting them in the DCEL
		Merge_Strategy vertex_merge_strategy = Merge_Strategy::AT_FIRST; ///< merging strategy when two verticies are equal
	};

} // O::Configuration


#endif //CONFIGURATION_DCEL_H