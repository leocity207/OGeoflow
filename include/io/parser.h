
#ifndef IO_PARSER_H
#define IO_PARSER_H

#include <filesystem>

#include "include/geojson/geojson.h"
#include "include/utils/expected.h"
#include "include/io/error.h"



namespace IO
{

	/**
	 * @brief Parse GeoJSON from a string
	 * @param json_string Input JSON string
	 * @return Parsed GeoJSON structure
	 * @throws Parse_Exception if parsing fails
	 */
	Util::Expected<GeoJSON::GeoJSON, IO::Error::Type> Parse_Geojson_String(const std::string& json_string);

	/**
	 * @brief Parse GeoJSON from a file
	 * @param filename Path to GeoJSON file
	 * @return Parsed GeoJSON structure
	 * @throws Parse_Exception if file cannot be read or parsing fails
	 */
	Util::Expected<GeoJSON::GeoJSON, IO::Error::Type> Parse_Geojson_File(const std::filesystem::path& filename);

} // namespace GeoJSON

#endif //IO_PARSER_H