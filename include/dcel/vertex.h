#ifndef DCEL_VERTEX_H
#define DCEL_VERTEX_H

// STL
#include <vector>
#include <cstdint>

// UTILS
#include <utils/unowned_ptr.h>

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

		/**
		 * @brief Produces a hash value for the vertex using its coordinates.
		 *
		 * This hash function converts the (x, y) coordinates into fixed-precision integers by multiplying them by 1e6 and rounding.
		 *
		 * @note The hash intentionally discards very large coordinate magnitudes because only the lower 32 bits of each rounded coordinate are kept.
		 * In practice this is acceptable, as we do not expect vertices to have extremely large coordinate values. Within typical coordinate ranges, the precision scaling (1e6) ensures stable and distinct hashes for different vertices.
		 *
		 * @return A 64-bit hash combining the rounded x and y coordinates.
		 */
		static uint64_t Hash(double x, double y) noexcept
		{
			long long fx = std::llround(x * 1e6);
			long long fy = std::llround(y * 1e6);
			return (static_cast<uint64_t>(static_cast<uint32_t>(fx)) << 32) | static_cast<uint64_t>(static_cast<uint32_t>(fy));
		}

		Vertex(double x, double y) :
			x(x),
			y(y),
			outgoing_edges()
		{
			outgoing_edges.reserve(2);
		}

		bool operator==(const Vertex& other)
		{
			return Hash(x, y) == Hash(other.x, other.y);
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
