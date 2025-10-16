#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/error/en.h>


#include "include/io/parser.h"
#include "include/io/exception.h"
#include "include/io/sax_handler.h"


GeoJSON::GeoJSON&& IO::Parse_Geojson_String(const std::string& json_string) {
	rapidjson::Reader reader;
	IO::GeoJSON_SAX_Handler handler;
	
	rapidjson::StringStream ss(json_string.c_str());
	
	if (!reader.Parse(ss, handler)) {
		rapidjson::ParseErrorCode error = reader.GetParseErrorCode();
		throw IO::Exception("JSON parsing error: " + std::string(rapidjson::GetParseError_En(error)) + " at offset " + std::to_string(reader.GetErrorOffset()));
	}
	
	if (!handler.Is_Valid()) {
		throw IO::Exception("GeoJSON parsing completed but result is invalid");
	}
	
	return handler.Get_Geojson();
}

GeoJSON::GeoJSON&& IO::Parse_Geojson_File(const std::filesystem::path& filename) {
    FILE* fp = fopen(filename.string().c_str(), "r");
    if (!fp) throw IO::Exception("Cannot open file: " + filename.string());

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    rapidjson::Reader reader;
    IO::GeoJSON_SAX_Handler handler;

    if (!reader.Parse(is, handler)) {
        rapidjson::ParseErrorCode error = reader.GetParseErrorCode();
        fclose(fp);
        throw IO::Exception(
            "JSON parsing error: " + std::string(rapidjson::GetParseError_En(error)) +
            " at offset " + std::to_string(reader.GetErrorOffset())
        );
    }

    fclose(fp);

    if (!handler.Is_Valid())
        throw IO::Exception("GeoJSON parsing completed but result is invalid");

    return handler.Get_Geojson();
}