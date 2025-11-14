#ifndef GEOJSON_BBOX_H
#define GEOJSON_BBOX_H

#include <array>
#include <variant>


namespace O::GeoJSON
{
	/**
	 * @brief Represents a GeoJSON Bounding Box.
	 *
	 * The Bbox structure stores the minimum and maximum coordinate extents
	 * of a geometry, feature, or feature collection.
	 * It can represent either 2D (4-element) or 3D (6-element) bounding boxes.
	 *
	 * @details
	 * - The order of coordinates is `[minX, minY, maxX, maxY]` for 2D,
	 *   or `[minX, minY, minZ, maxX, maxY, maxZ]` for 3D.
	 * - Accessors are provided for both constant and mutable access.
	 *
	 * @see https://datatracker.ietf.org/doc/html/rfc7946#section-5
	 */
	struct Bbox
	{
		/// @brief Variant representing either 2D or 3D bounding box coordinates.
		std::variant<std::array<double, 4>, std::array<double, 6>> coordinates;

		/// @brief Checks if the bounding box includes altitude values (3D).
		bool Has_Altitude() const noexcept { return std::holds_alternative<std::array<double,6>>(coordinates); }

		/// @name Accessors
		/// @brief Access the bounding box coordinates (2D or 3D).
		/// @{
		const std::array<double,4>& Get() const noexcept { return std::get<std::array<double,4>>(coordinates); }
		const std::array<double,6>& Get_With_Altitudes() const noexcept { return std::get<std::array<double,6>>(coordinates); }

		std::array<double,4>& Get() noexcept { return std::get<std::array<double,4>>(coordinates); }
		std::array<double,6>& Get_With_Altitudes() noexcept { return std::get<std::array<double,6>>(coordinates); }
		/// @}
	};
}

#endif // GEOJSON_BBOX_H
