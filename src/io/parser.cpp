#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>


#include "include/io/parser.h"
#include "include/io/full_parser.h"

using Expected_Type = Util::Expected<GeoJSON::GeoJSON, GeoJSON::IO::Error::Type>;
Expected_Type GeoJSON::IO::Parse_Geojson_String(const std::string& json_string) {
	rapidjson::Reader reader;
	::GeoJSON::IO::Full_Parser handler;
	
	rapidjson::StringStream ss(json_string.c_str());
	
	if (!reader.Parse(ss, handler))
    {
        if (handler.Get_Error() != ::GeoJSON::IO::Error::Type::NO_ERROR)
            return Expected_Type::Make_Error(handler.Get_Error());
        else
            return Expected_Type::Make_Error(::GeoJSON::IO::Error::Type::PARSING_ERROR);
	}
    if (auto geojson = handler.Get_Geojson())
    {
        return Expected_Type::Make_Value(std::move(*geojson));
    }
    if (handler.Get_Error() != ::GeoJSON::IO::Error::Type::NO_ERROR)
        return Expected_Type::Make_Error(handler.Get_Error());
    else
        return Expected_Type::Make_Error(::GeoJSON::IO::Error::Type::PARSING_ERROR);
	
}

Expected_Type GeoJSON::IO::Parse_Geojson_File(const std::filesystem::path& filename) {
    auto fp = std::unique_ptr<FILE,decltype(&fclose)>(fopen(filename.string().c_str(), "r"),fclose);
    if(!fp)  Expected_Type::Make_Error(::GeoJSON::IO::Error::Type::FILE_OPENNING_FAILED);

    char readBuffer[65536];
    rapidjson::FileReadStream is(fp.get(), readBuffer, sizeof(readBuffer));

    rapidjson::Reader reader;
    ::GeoJSON::IO::Full_Parser handler;

    if (!reader.Parse(is, handler))
    {
        if (handler.Get_Error() != ::GeoJSON::IO::Error::Type::NO_ERROR)
            return Expected_Type::Make_Error(handler.Get_Error());
        else
            return Expected_Type::Make_Error(::GeoJSON::IO::Error::Type::PARSING_ERROR);
    }
    if (auto geojson = handler.Get_Geojson())
    {
        return Expected_Type::Make_Value(std::move(*geojson));
    }
    if (handler.Get_Error() != ::GeoJSON::IO::Error::Type::NO_ERROR)
        return Expected_Type::Make_Error(handler.Get_Error());
    else
        return Expected_Type::Make_Error(::GeoJSON::IO::Error::Type::PARSING_ERROR);
}