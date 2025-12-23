#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <format>
#include <ranges>

#include "io/sax_parser.h"
#include "io/error.h"

#include "geojson/properties.h"
#include "geojson/position.h"
#include <optional>
#include "geojson/geometry_type/geometry_collection.h"

using Level1 = O::GeoJSON::Position;
using Level2 = std::vector<O::GeoJSON::Position>;
using Level3 = std::vector<std::vector<O::GeoJSON::Position>>;
using Level4 = std::vector<O::GeoJSON::Polygon>;

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::On_Geometry(O::GeoJSON::Geometry&& geometry, std::size_t element_number) 
{
	return static_cast<Derived&>(*this).On_Geometry(std::move(geometry), element_number);
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::On_Feature(O::GeoJSON::Feature&& feature) 
{
	return static_cast<Derived&>(*this).On_Feature(std::move(feature));
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::On_Feature_Collection(std::optional<O::GeoJSON::Bbox>&& bbox, std::optional<std::string>&& id)
{
	return static_cast<Derived&>(*this).On_Feature_Collection(std::move(bbox), std::move(id));
}


template<class Derived>
O::GeoJSON::IO::SAX_Parser<Derived>::SAX_Parser() :
	m_current_error(O::GeoJSON::IO::Error::NO_ERROR),
	m_level(0),
	m_max_level(0),
	m_add_level(0)
{
	Push_Context(Parse_State::ROOT);
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Push_Context(Parse_State state,O::GeoJSON::Property& ref_property, std::string_view key)
{
	switch (state)
	{
		case Parse_State::FEATURE:
			m_id = std::nullopt;
			break;
		case Parse_State::COORDINATES:
			m_coordinate = O::GeoJSON::Position();
			m_positions.Clear();
			m_max_level = 0;
			m_level = 0;
			m_add_level = 0;
			break;
	}
	m_context_stack.emplace_back( ref_property, state, std::string(key), O::GeoJSON::Key::FOREIGN);
	return true;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Reset_State(Parse_State state)
{
	Current_Context().state = state;
	return true;
}


template<class Derived>
typename O::GeoJSON::IO::SAX_Parser<Derived>::Parse_Context O::GeoJSON::IO::SAX_Parser<Derived>::Pop_Context()
{
	assert(!m_context_stack.empty());
	typename SAX_Parser<Derived>::Parse_Context context = std::move(m_context_stack.back());
	m_context_stack.pop_back();
	return context;
}

template<class Derived>
typename O::GeoJSON::IO::SAX_Parser<Derived>::Parse_State O::GeoJSON::IO::SAX_Parser<Derived>::Current_State() const
{
	assert(!m_context_stack.empty());
	return m_context_stack.back().state;
}

template<class Derived>
typename O::GeoJSON::IO::SAX_Parser<Derived>::Parse_Context& O::GeoJSON::IO::SAX_Parser<Derived>::Current_Context()
{
	assert(!m_context_stack.empty());
	return m_context_stack.back();
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::In_Array()
{
	switch (Current_State())
	{
		case Parse_State::PROPERTIES_SUB_ARRAY:
		case Parse_State::FOREIGN_ARRAY:
		case Parse_State::COORDINATES:
		case Parse_State::FEATURE_COLLECTION:
		case Parse_State::GEOMETRY_COLLECTION:
		case Parse_State::BBOX:
			return true;
		default:
			return false;
	}
}

template<class Derived>
O::GeoJSON::Key O::GeoJSON::IO::SAX_Parser<Derived>::Set_Current_Key(std::string_view key)
{
	if (!m_context_stack.empty())
	{
		m_context_stack.back().key = O::GeoJSON::String_To_Key(key);
		return m_context_stack.back().key;
	}
	return O::GeoJSON::Key::FOREIGN;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::StartObject()
{
	switch (Current_State())
	{
		case Parse_State::ROOT:
		case Parse_State::PROPERTIES_OBJECT:    return true;
		case Parse_State::PROPERTIES_SUB_ARRAY:
		{
			if (Current_Context().property.get().Is_Array())
			{
				Current_Context().property.get().Get_Array().emplace_back(O::GeoJSON::Property::Object());
				return Push_Context(Parse_State::PROPERTIES_OBJECT, Current_Context().property.get().Get_Array().back());
			}
			return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
		}
		case Parse_State::PROPERTIES_SUB_KEY:
		{
			if (Current_Context().property.get().Is_Object())
			{
				if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
					return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
				Current_Context().property.get().Get_Object()[Current_Context().key_str] = O::GeoJSON::Property::Object();
				Reset_State(Parse_State::PROPERTIES_OBJECT);
				Current_Context().property = Current_Context().property.get().Get_Object()[Current_Context().key_str];
				return true;
			}
			return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
		}
		case Parse_State::FOREIGN_KEY:          return Reset_State(Parse_State::FOREIGN_OBJECT);
		case Parse_State::FOREIGN_ARRAY:        return Push_Context(Parse_State::FOREIGN_OBJECT);
		case Parse_State::FEATURE_COLLECTION:   return Push_Context(Parse_State::FEATURE);
		case Parse_State::GEOMETRY_COLLECTION:  return Push_Context(Parse_State::GEOMETRY);
		case Parse_State::GEOMETRY: return true;
		case Parse_State::FEATURE:  return true;
	}
	return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_OBJECT);
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::EndObject(rapidjson::SizeType /*elementCount*/)
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_OBJECT:
	case Parse_State::FOREIGN_OBJECT:
		Pop_Context();
		return true;
	case Parse_State::GEOMETRY:
		if (auto geometry = Create_Geometry())
			return On_Geometry(std::move(*geometry), Pop_Context().geometry_count);
		return false;
	case Parse_State::FEATURE:
		if (auto feature = Create_Feature())
		{
			Pop_Context();
			return On_Feature(std::move(*feature));
		}
		return false;
	case Parse_State::ROOT:
		switch (Current_Context().type)
		{
		case O::GeoJSON::Type::FEATURE_COLLECTION:
			return On_Feature_Collection(std::move(Current_Context().bbox), std::move(m_id));
		case O::GeoJSON::Type::FEATURE:
			if (auto feature = Create_Feature())
				return On_Feature(std::move(*feature));
			return false;
		case O::GeoJSON::Type::POINT:
		case O::GeoJSON::Type::MULTI_POINT:
		case O::GeoJSON::Type::LINE_STRING:
		case O::GeoJSON::Type::MULTI_LINE_STRING:
		case O::GeoJSON::Type::POLYGON:
		case O::GeoJSON::Type::MULTI_POLYGON:
		case O::GeoJSON::Type::GEOMETRY_COLLECTION:
			if (auto geometry = Create_Geometry())
				return On_Geometry(std::move(*geometry), Current_Context().geometry_count);
			return false;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_KEY);
	}
	return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_OBJECT);
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::StartArray() {
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_ARRAY:
	{
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(O::GeoJSON::Property::Array());
			return Push_Context(Parse_State::PROPERTIES_SUB_ARRAY, Current_Context().property.get().Get_Array().back());
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	}
	case Parse_State::PROPERTIES_SUB_KEY:
	{
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = O::GeoJSON::Property::Array();
			Reset_State(Parse_State::PROPERTIES_SUB_ARRAY);
			Current_Context().property = Current_Context().property.get().Get_Object()[Current_Context().key_str];
			return true;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	}
	case Parse_State::FOREIGN_KEY:          return Reset_State(Parse_State::FOREIGN_ARRAY);
	case Parse_State::FOREIGN_ARRAY:        return Push_Context(Parse_State::FOREIGN_ARRAY);
	case Parse_State::FEATURE_COLLECTION:
	case Parse_State::GEOMETRY_COLLECTION:
	case Parse_State::BBOX:                 return true;
	case Parse_State::COORDINATES:
	{
		m_positions.Clear();
		m_level++;
		m_max_level = std::max(m_max_level, m_level);
		return true;
	}
	default: return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_ARRAY);
	}
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::EndArray(rapidjson::SizeType element_count)
{
	switch (Current_State())
	{
		case Parse_State::PROPERTIES_SUB_ARRAY:
		case Parse_State::FOREIGN_ARRAY:
		case Parse_State::FEATURE_COLLECTION:
			Pop_Context();
			return true;
		case Parse_State::GEOMETRY_COLLECTION:
			Current_Context().geometry = std::move(Pop_Context().geometry);
			Current_Context().geometry_count = element_count;
			return true;
		case Parse_State::BBOX:
		{
			if (m_positions.Size() == 4)
			{
				Pop_Context();
				Current_Context().bbox = O::GeoJSON::Bbox{ std::array<double, 4>()};
				for(auto i : std::views::iota(0,4))
					std::get<std::array<double, 4>>(Current_Context().bbox->coordinates)[i] = m_positions[i];
				return true;
			}
			else if(m_positions.Size() == 6)
			{
				Pop_Context();
				Current_Context().bbox = O::GeoJSON::Bbox{ std::array<double, 6>() };
				for (auto i : std::views::iota(0, 6))
					std::get<std::array<double, 6>>(Current_Context().bbox->coordinates)[i] = m_positions[i];
				return true;
			}
			return Push_Error(O::GeoJSON::IO::Error::BBOX_SIZE_INCONSISTENT);	
		}
		case Parse_State::COORDINATES:
		{
			if (!Finalize_Coordinates())
				return false;
			m_level--;
			if (m_level == 0)
				Pop_Context();
			return true;
		}
		default: return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_ARRAY);
	}
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Key(const char* str, rapidjson::SizeType length, bool /*copy*/)
{
	std::string_view key_str(str, length);
	O::GeoJSON::Key key = Set_Current_Key(key_str);
	
	auto current = Current_State();
	
	switch (current)
	{
		case Parse_State::FOREIGN_OBJECT: return Push_Context(Parse_State::FOREIGN_KEY);
		case Parse_State::PROPERTIES_OBJECT: return Push_Context(Parse_State::PROPERTIES_SUB_KEY, Current_Context().property, key_str);
		case Parse_State::GEOMETRY:
			switch (key)
			{
				case O::GeoJSON::Key::TYPE:                return Push_Context(Parse_State::TYPE);
				case O::GeoJSON::Key::COORDINATES:         return Push_Context(Parse_State::COORDINATES);
				case O::GeoJSON::Key::BBOX:                return Push_Context(Parse_State::BBOX);
				case O::GeoJSON::Key::FOREIGN:             return Push_Context(Parse_State::FOREIGN_KEY);
				case O::GeoJSON::Key::GEOMETRY_COLLECTION: return Push_Context(Parse_State::GEOMETRY_COLLECTION);
			}
			break;
		case Parse_State::FEATURE:
			switch (key)
			{
				case O::GeoJSON::Key::TYPE:       return Push_Context(Parse_State::TYPE);
				case O::GeoJSON::Key::GEOMETRY:   return Push_Context(Parse_State::GEOMETRY);
				case O::GeoJSON::Key::PROPERTIES:
					m_property = O::GeoJSON::Property::Object();
					return Push_Context(Parse_State::PROPERTIES_OBJECT, m_property);
				case O::GeoJSON::Key::BBOX:       return Push_Context(Parse_State::BBOX);
				case O::GeoJSON::Key::FOREIGN:    return Push_Context(Parse_State::FOREIGN_KEY);
				case O::GeoJSON::Key::ID:         return Push_Context(Parse_State::ID);
			}
			break;
		case Parse_State::ROOT:
			switch (key)
			{
				case O::GeoJSON::Key::TYPE:                return Push_Context(Parse_State::TYPE);
				case O::GeoJSON::Key::FEATURES_COLLECTION: return Push_Context(Parse_State::FEATURE_COLLECTION);
				case O::GeoJSON::Key::PROPERTIES:
					m_property = O::GeoJSON::Property::Object();
					return Push_Context(Parse_State::PROPERTIES_OBJECT, m_property);
				case O::GeoJSON::Key::BBOX:                return Push_Context(Parse_State::BBOX);
				case O::GeoJSON::Key::COORDINATES:         return Push_Context(Parse_State::COORDINATES);
				case O::GeoJSON::Key::FOREIGN:             return Push_Context(Parse_State::FOREIGN_KEY);
				case O::GeoJSON::Key::GEOMETRY:            return Push_Context(Parse_State::GEOMETRY);
				case O::GeoJSON::Key::ID:                  return Push_Context(Parse_State::ID);
				case O::GeoJSON::Key::GEOMETRY_COLLECTION: return Push_Context(Parse_State::GEOMETRY_COLLECTION);
			}
	}
	return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_KEY);
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::String(const char* str, rapidjson::SizeType length, bool /*copy*/)
{
	switch (Current_State())
	{		
		case Parse_State::TYPE:
			Pop_Context();
			Current_Context().type = O::GeoJSON::String_To_Type(std::string_view(str, length));
			if (Current_Context().type == O::GeoJSON::Type::UNKNOWN)
			{
				Push_Error(O::GeoJSON::IO::Error::UNKNOWN_TYPE);
				return false;
			}
			return true;
		case Parse_State::ID:
			m_id = std::string(str, length);
			break;
		case Parse_State::PROPERTIES_SUB_KEY:
			if (Current_Context().property.get().Is_Object())
			{
				if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
					return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
				Current_Context().property.get().Get_Object()[Current_Context().key_str] = std::string(str, length);
				break;
			}
			return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
		case Parse_State::PROPERTIES_SUB_ARRAY:
			if (Current_Context().property.get().Is_Array())
			{
				Current_Context().property.get().Get_Array().emplace_back(std::string(str, length));
				break;
			}
			return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
		case Parse_State::FOREIGN_ARRAY: break;
		case Parse_State::FOREIGN_KEY:   break;
		default:
			return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_VALUE);
	}
	if(!In_Array())
		Pop_Context();
	return true;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Bool(bool value)
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = value;
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(value);
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::FOREIGN_ARRAY: break;
	case Parse_State::FOREIGN_KEY:   break;
	default:
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_VALUE);
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Double(double value)
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = value;
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(value);
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	case Parse_State::COORDINATES:
	case Parse_State::BBOX:
		if (m_level != m_max_level)
			return Push_Error(O::GeoJSON::IO::Error::INCONSCISTENT_COORDINATE_LEVEL);
		m_positions.Emplace_Back(value);
		break;
	case Parse_State::FOREIGN_ARRAY:
	case Parse_State::FOREIGN_KEY: break;
	default:
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

// Implement other numeric types to handle coordinates
template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Int(int value) 
{ 
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = value;
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(value);
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Uint(unsigned value) 
{ 
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = static_cast<int>(value);
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(static_cast<int>(value));
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Int64(int64_t value) 
{ 
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = static_cast<int>(value);
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(static_cast<int>(value));
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Uint64(uint64_t value) 
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = static_cast<int>(value);
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(static_cast<int>(value));
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::ID:
		m_id = std::to_string(value);
		break;
	default:
		return Double(static_cast<double>(value));
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::RawNumber(const char* str, rapidjson::SizeType length, bool /*copy*/) {
	// Convert string to double
	try
	{
		double value = std::stod(std::string(str, length));
		return Double(value);
	} 
	catch (const std::exception&)
	{
		return false;
	}
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Null()
{
	switch (Current_State())
	{
	case Parse_State::PROPERTIES_SUB_KEY:
		if (Current_Context().property.get().Is_Object())
		{
			if (Current_Context().property.get().Get_Object().contains(Current_Context().key_str))
				return Push_Error(O::GeoJSON::IO::Error::PROPERTY_KEY_ALREADY_EXIST);
			Current_Context().property.get().Get_Object()[Current_Context().key_str] = O::GeoJSON::Property();
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_SUB_ARRAY:
		if (Current_Context().property.get().Is_Array())
		{
			Current_Context().property.get().Get_Array().emplace_back(O::GeoJSON::Property());
			break;
		}
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_PROPERTY_STATE);
	case Parse_State::PROPERTIES_OBJECT:
	case Parse_State::GEOMETRY:
	case Parse_State::FOREIGN_ARRAY:
	case Parse_State::FOREIGN_KEY: break;
	default:
		return Push_Error(O::GeoJSON::IO::Error::UNEXPECTED_STATE_VALUE);
	}
	if (!In_Array())
		Pop_Context();
	return true;
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Finalize_Coordinates()
{
	// verify current pushed positions
	if (m_level != m_max_level)
	{
		m_add_level = m_max_level - m_level;
		if (m_positions.Size()) return Push_Error(O::GeoJSON::IO::Error::INCONSCISTENT_COORDINATE_LEVEL);
		return true;
	}
	else if (m_positions.Size() < 2) return Push_Error(O::GeoJSON::IO::Error::COORDINATE_UNDERSIZED);
	else if (m_positions.Size() > 3) return Push_Error(O::GeoJSON::IO::Error::COORDINATE_OVERSIZED);
	
	auto position = Level1{ m_positions[0], m_positions[1], (m_positions.Size() == 3) ? std::optional<double>(m_positions[2]) : std::nullopt };
	m_positions.Clear();
	switch(m_max_level)
	{
		case 1:
			m_coordinate = std::move(position);
			return true;
		case 2:
		{
			if (!std::holds_alternative<Level2>(m_coordinate)) { m_coordinate = Level2{}; };
			Level2& obj = std::get<Level2>(m_coordinate);
			obj.emplace_back(std::move(position));
			return true;
		}
		case 3:
		{
			if (!std::holds_alternative<Level3>(m_coordinate)) { m_coordinate = Level3{ Level2{} }; };
			Level3& obj = std::get<Level3>(m_coordinate);
			if (m_add_level == 0) {}
			else if (m_add_level == 1)
			{
				obj.emplace_back(Level2{});
				m_add_level = false;
			}
			else
				return Push_Error(O::GeoJSON::IO::Error::INCONSCISTENT_COORDINATE_LEVEL);
			obj.back().emplace_back(std::move(position));
			return true;
		}
		case 4:
		{
			if (!std::holds_alternative<Level4>(m_coordinate)) { m_coordinate = Level4{ O::GeoJSON::Polygon{Level3{Level2{}}} }; };
			Level4& obj = std::get<Level4>(m_coordinate);
			if (m_add_level == 0) {}
			else if (m_add_level == 1)
			{
				obj.back().rings.emplace_back(Level2{});
				m_add_level = false;
			}
			else if (m_add_level == 2)
			{
				obj.emplace_back(O::GeoJSON::Polygon{ Level3{Level2{}} });
				m_add_level = false;
			}
			else
				return Push_Error(O::GeoJSON::IO::Error::INCONSCISTENT_COORDINATE_LEVEL);
			obj.back().rings.back().emplace_back(std::move(position));
			return true;
		}
		default: return false;
	}
}

template<class Derived>
std::optional<O::GeoJSON::Feature> O::GeoJSON::IO::SAX_Parser<Derived>::Create_Feature()
{
	if(!Current_Context().geometry.has_value())
	{
		if(auto bbox = Current_Context().bbox)
		{
			if(auto& id = m_id)
				return O::GeoJSON::Feature{std::move(m_context_stack.back().geometry),std::move(m_property), std::move(*id), std::move(*bbox)};
			else
				return O::GeoJSON::Feature{std::move(m_context_stack.back().geometry),std::move(m_property), std::nullopt, std::move(*bbox)};
		}
		else if(auto& id = m_id)
			return O::GeoJSON::Feature{std::move(m_context_stack.back().geometry),std::move(m_property), std::move(*id), std::nullopt};
		else
			return O::GeoJSON::Feature{std::move(m_context_stack.back().geometry), std::move(m_property), std::nullopt, std::nullopt};
	}
	Push_Error(O::GeoJSON::IO::Error::FEATURE_NEED_INITIALIZED_GEOMETRY);
	return std::nullopt;
}

template<class Derived>
std::optional<O::GeoJSON::Geometry> O::GeoJSON::IO::SAX_Parser<Derived>::Create_Geometry()
{
	auto Fail = [&](Error e) -> std::optional<O::GeoJSON::Geometry>
	{
		Push_Error(e);
		return std::nullopt;
	};


	auto Make_Multi_Line_String = [](Level3 & coordinates) -> O::GeoJSON::Multi_Line_String
	{
		std::vector<O::GeoJSON::Line_String> lines;
		for (auto&& coordiante : coordinates)
			lines.emplace_back(O::GeoJSON::Line_String{ std::move(coordiante) });
		return O::GeoJSON::Multi_Line_String{ std::move(lines) };
	};

	auto Make_Multi_Polygon = [](Level4& coordinates) -> O::GeoJSON::Multi_Polygon
	{
		std::vector<O::GeoJSON::Polygon> polygones;
		for (auto&& coordiante : coordinates)
			polygones.emplace_back(O::GeoJSON::Polygon{ std::move(coordiante) });
		return O::GeoJSON::Multi_Polygon{ std::move(polygones) };
	};

	switch (Current_Context().type)
	{
		case O::GeoJSON::Type::POINT:
			if (!std::holds_alternative<Level1>(m_coordinate))
				return Fail(O::GeoJSON::IO::Error::BAD_COORDINATE_FOR_GEMETRY);
			return O::GeoJSON::Geometry{ O::GeoJSON::Point{std::move(std::get<Level1>(m_coordinate))}, std::move(Current_Context().bbox)};

		case O::GeoJSON::Type::MULTI_POINT:
			if (!std::holds_alternative<Level2>(m_coordinate))
				return Fail(O::GeoJSON::IO::Error::BAD_COORDINATE_FOR_GEMETRY);
			return O::GeoJSON::Geometry{ O::GeoJSON::Multi_Point{std::move(std::get<Level2>(m_coordinate))}, std::move(Current_Context().bbox) };

		case O::GeoJSON::Type::LINE_STRING:
			if (!std::holds_alternative<Level2>(m_coordinate))
				return Fail(O::GeoJSON::IO::Error::BAD_COORDINATE_FOR_GEMETRY);
			if (std::get<Level2>(m_coordinate).size() < 2)
				return Fail(O::GeoJSON::IO::Error::NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING);
			return O::GeoJSON::Geometry{ O::GeoJSON::Line_String{std::move(std::get<Level2>(m_coordinate))}, std::move(Current_Context().bbox) };

		case O::GeoJSON::Type::MULTI_LINE_STRING:
			if (!std::holds_alternative<Level3>(m_coordinate))
				return Fail(O::GeoJSON::IO::Error::BAD_COORDINATE_FOR_GEMETRY);
			for(auto& line_string : std::get<Level3>(m_coordinate))
				if(line_string.size() < 2)
					return Fail(O::GeoJSON::IO::Error::NEED_AT_LEAST_TWO_POSITION_FOR_LINESTRING);
			return O::GeoJSON::Geometry{ Make_Multi_Line_String(std::get<Level3>(m_coordinate)), std::move(Current_Context().bbox) };

		case O::GeoJSON::Type::POLYGON:
			if (!std::holds_alternative<Level3>(m_coordinate))
				return Fail(O::GeoJSON::IO::Error::BAD_COORDINATE_FOR_GEMETRY);
			if (std::get<Level3>(m_coordinate).empty())
				return Fail(O::GeoJSON::IO::Error::POLYGON_NEED_AT_LEAST_ONE_RING);
			for (auto& ring : std::get<Level3>(m_coordinate))
			{
				if (ring.size() < 4)
					return Fail(O::GeoJSON::IO::Error::NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON);
				if (ring.front().altitude != ring.back().altitude || ring.front().latitude != ring.back().latitude || ring.front().longitude != ring.back().longitude)
					return Fail(O::GeoJSON::IO::Error::POLYGON_NEED_TO_BE_CLOSED);
			}
			return O::GeoJSON::Geometry{ O::GeoJSON::Polygon(std::get<Level3>(m_coordinate)), std::move(Current_Context().bbox) };

		case O::GeoJSON::Type::MULTI_POLYGON:
			if (!std::holds_alternative<Level4>(m_coordinate))
				return Fail(O::GeoJSON::IO::Error::BAD_COORDINATE_FOR_GEMETRY);
			for (auto& polygone : std::get<Level4>(m_coordinate))
			{
				if(polygone.rings.empty())
					return Fail(O::GeoJSON::IO::Error::POLYGON_NEED_AT_LEAST_ONE_RING);
				for (auto& ring : polygone.rings)
				{
					if (ring.size() < 4)
						return Fail(O::GeoJSON::IO::Error::NEED_AT_LEAST_FOUR_POSITION_FOR_POLYGON);
					if (ring.front().altitude != ring.back().altitude || ring.front().latitude != ring.back().latitude || ring.front().longitude != ring.back().longitude)
						return Fail(O::GeoJSON::IO::Error::POLYGON_NEED_TO_BE_CLOSED);
				}
			}
			return O::GeoJSON::Geometry{ Make_Multi_Polygon(std::get<Level4>(m_coordinate)) , std::move(Current_Context().bbox) };

		case O::GeoJSON::Type::GEOMETRY_COLLECTION:
			return O::GeoJSON::Geometry{ O::GeoJSON::Geometry_Collection{}, std::move(Current_Context().bbox) };
		default:
			Push_Error(O::GeoJSON::IO::Error::UNKNOWN_GEOMETRY_TYPE);
			return std::nullopt;
	}
}

template<class Derived>
bool O::GeoJSON::IO::SAX_Parser<Derived>::Push_Error(Error error)
{
	 m_current_error = error; return false; 
}