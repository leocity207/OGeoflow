#include "include/geojson/json_value.h"
#include "include/geojson/exception.h"

bool GeoJSON::JSON_Value::is_null() const 
{ 
	return std::holds_alternative<std::monostate>(value); 
}

bool GeoJSON::JSON_Value::is_bool() const 
{ 
	return std::holds_alternative<bool>(value); 
}

bool GeoJSON::JSON_Value::is_number() const 
{ 
	return std::holds_alternative<double>(value); 
}

bool GeoJSON::JSON_Value::is_string() const 
{ 
	return std::holds_alternative<std::string>(value); 
}

bool GeoJSON::JSON_Value::is_array() const 
{ 
	return std::holds_alternative<JSON_Array>(value); 
}

bool GeoJSON::JSON_Value::is_object() const 
{ 
	return std::holds_alternative<JSON_Object>(value); 
}

const GeoJSON::JSON_Object& GeoJSON::JSON_Value::as_object() const 
{ 
	if (!is_object()) throw GeoJSON::Exception("Value is not an object");
	return std::get<GeoJSON::JSON_Object>(value); 
}

const GeoJSON::JSON_Array& GeoJSON::JSON_Value::as_array() const 
{ 
	if (!is_array()) throw GeoJSON::Exception("Value is not an array");
	return std::get<GeoJSON::JSON_Array>(value); 
}

const std::string& GeoJSON::JSON_Value::as_string() const
{ 
	if (!is_string()) throw GeoJSON::Exception("Value is not a string");
	return std::get<std::string>(value); 
}

double GeoJSON::JSON_Value::as_number() const 
{ 
	if (!is_number()) throw GeoJSON::Exception("Value is not a number");
	return std::get<double>(value); 
}

bool GeoJSON::JSON_Value::as_bool() const 
{ 
	if (!is_bool()) throw GeoJSON::Exception("Value is not a boolean");
	return std::get<bool>(value); 
}