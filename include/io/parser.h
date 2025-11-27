
#ifndef IO_PARSER_H
#define IO_PARSER_H

#include <filesystem>

#include "include/geojson/root.h"
#include <utils/expected.h>
#include "error.h"

namespace O::GeoJSON::IO
{
	/**
	 * @brief Parse GeoJSON from a string
	 * @param json_string Input JSON string
	 * @return Parsed GeoJSON structure
	 */
	O::Expected<O::GeoJSON::Root, Error> Parse_Geojson_String(const std::string& json_string);

	/**
	 * @brief Parse GeoJSON from a file
	 * @param filename Path to GeoJSON file
	 * @return Parsed GeoJSON structure
	 */
	O::Expected<O::GeoJSON::Root, Error> Parse_Geojson_File(const std::filesystem::path& filename);

} // namespace GeoJSON

#endif //IO_PARSER_H