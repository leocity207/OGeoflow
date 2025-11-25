#ifndef GEOJSON_BBOX_H
#define GEOJSON_BBOX_H

#include <array>
#include <variant>


namespace O::GeoJSON
{
	struct Bbox
	{
		std::variant<std::array<double,4>,std::array<double,6>> coordinates;

		bool Has_Altitude() const noexcept { return std::holds_alternative<std::array<double,6>>(coordinates); }

		const std::array<double,4>& Get()                 const noexcept { return std::get<std::array<double,4>>(coordinates); }
		const std::array<double,6>& Get_With_Altitudes()  const noexcept { return std::get<std::array<double,6>>(coordinates); }
		std::array<double,4>& Get()                 noexcept { return std::get<std::array<double,4>>(coordinates); }
		std::array<double,6>& Get_With_Altitudes()  noexcept { return std::get<std::array<double,6>>(coordinates); }
	};
}

#endif //GEOJSON_BBOX_H