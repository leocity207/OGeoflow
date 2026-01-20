#ifndef DCEL_VERTEX_H
#define DCEL_VERTEX_H

// STL
#include <vector>
#include <cstdint>

// UTILS
#include <utils/unowned_ptr.h>

// CONFIGURATION
#include "configuration/dcel.h"

namespace O::DCEL
{

	/**
	 * @brief Vertex represent a Point in space linked to some outgoing and ingoing half_edges.
	 */
	template< class Half_Edge>
	struct Vertex
	{
		double x = 0.0;  ///< x coordinate of the vertex (it is assumed that this value is in the same Coordinate system as the GeoJSON).
		double y = 0.0;  ///< y coordinate of the vertex (it is assumed that this value is in the same Coordinate system as the GeoJSON).
		std::vector<O::Unowned_Ptr<Half_Edge>> outgoing_edges; ///< ordered outgoing half edges of the vertex (hedges are ordered clockwise).

		Vertex(double x, double y) :
			x(x),
			y(y),
			outgoing_edges()
		{
			outgoing_edges.reserve(2);
		}

		/**
		 * @brief Move the vertex
		 * @param new_x new_coordinate
		 * @param new_y 
		 * @note does not check if the DCEL containing the moved vertex is valid
		 * @return true always
		 */
		bool Move(double new_x, double new_y)
		{
			x = new_x;
			y = new_y;
			return true;
		}
	};

} // namespace O::DCEL

#endif // DCEL_VERTEX_H
