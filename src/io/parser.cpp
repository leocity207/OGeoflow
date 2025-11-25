#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>


#include "include/io/parser.h"
#include "include/io/full_parser.h"


O::Expected<O::GeoJSON::Root, O::GeoJSON::IO::Error> O::GeoJSON::IO::Parse_Geojson_String(const std::string& json_string)
{
	rapidjson::Reader reader;
	Full_Parser handler;
	
	rapidjson::StringStream ss(json_string.c_str());
	
	if (!reader.Parse(ss, handler))
	{
		if (handler.Get_Error() != Error::NO_ERROR)
			return O::Expected<Root, Error>::Make_Error(handler.Get_Error());
		else
			return O::Expected<Root, Error>::Make_Error(Error::PARSING_ERROR);
	}
	if (auto geojson = handler.Get_Geojson())
		return O::Expected<Root, Error>::Make_Value(std::move(*geojson));
	if (handler.Get_Error() != Error::NO_ERROR)
		return O::Expected<Root, Error>::Make_Error(handler.Get_Error());
	else
		return O::Expected<Root, Error>::Make_Error(Error::PARSING_ERROR);
	
}

O::Expected<O::GeoJSON::Root, O::GeoJSON::IO::Error> O::GeoJSON::IO::Parse_Geojson_File(const std::filesystem::path& filename)
{
	auto fp = std::unique_ptr<FILE,decltype(&fclose)>(fopen(filename.string().c_str(), "r"),fclose);
	if(!fp)  O::Expected<Root, Error>::Make_Error(Error::FILE_OPENNING_FAILED);

	char readBuffer[65536];
	rapidjson::FileReadStream is(fp.get(), readBuffer, sizeof(readBuffer));

	rapidjson::Reader reader;
	Full_Parser handler;

	if (!reader.Parse(is, handler))
	{
		if (handler.Get_Error() != Error::NO_ERROR)
			return O::Expected<Root, Error>::Make_Error(handler.Get_Error());
		else
			return O::Expected<Root, Error>::Make_Error(Error::PARSING_ERROR);
	}
	if (auto geojson = handler.Get_Geojson())
		return O::Expected<Root, Error>::Make_Value(std::move(*geojson));
	if (handler.Get_Error() != Error::NO_ERROR)
		return O::Expected<Root, Error>::Make_Error(handler.Get_Error());
	else
		return O::Expected<Root, Error>::Make_Error(Error::PARSING_ERROR);
}