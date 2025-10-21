#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>


#include "include/io/parser.h"
#include "include/io/sax_handler.h"

using Expected_Type = Util::Expected<GeoJSON::GeoJSON, IO::Error::Type>;
Expected_Type IO::Parse_Geojson_String(const std::string& json_string) {
	rapidjson::Reader reader;
	IO::GeoJSON_SAX_Handler handler;
	
	rapidjson::StringStream ss(json_string.c_str());
	
	if (!reader.Parse(ss, handler))
    {
        if (handler.Get_Error() != IO::Error::Type::NO_ERROR)
            return Expected_Type::Make_Error(handler.Get_Error());
        else
            return Expected_Type::Make_Error(IO::Error::Type::PARSING_ERROR);
	}
	
	return Expected_Type::Make_Value(std::move(handler.Get_Geojson()));
}

Expected_Type IO::Parse_Geojson_File(const std::filesystem::path& filename) {
    auto fp = std::unique_ptr<FILE,decltype(&fclose)>(fopen(filename.string().c_str(), "r"),fclose);
    if(!fp)  Expected_Type::Make_Error(IO::Error::Type::FILE_OPENNING_FAILED);

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp.get(), readBuffer, sizeof(readBuffer));

    rapidjson::Reader reader;
    IO::GeoJSON_SAX_Handler handler;

    if (!reader.Parse(is, handler))
    {
        if (handler.Get_Error() != IO::Error::Type::NO_ERROR)
            return Expected_Type::Make_Error(handler.Get_Error());
        else
            return Expected_Type::Make_Error(IO::Error::Type::PARSING_ERROR);
    }
    return Expected_Type::Make_Value(std::move(handler.Get_Geojson()));
}