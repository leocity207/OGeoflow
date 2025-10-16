#include "include/geojson/position.h"


GeoJSON::Position::Position() :
	longitude(0.0),
	latitude(0.0),
	altitude(std::nullopt)
{

}

GeoJSON::Position::Position(double lon, double lat) :
	longitude(lon),
	latitude(lat),
	altitude(std::nullopt)
{

}

GeoJSON::Position::Position(double lon, double lat, double alt) :
	longitude(lon),
	latitude(lat),
	altitude(alt)
{

}