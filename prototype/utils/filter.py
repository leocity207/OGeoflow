from typing import Dict, Any, List

class Filter:

	#: string of the underlying geojson
	_json: Dict[str,Any]

	def __init__(self, json):
		self._json = json

	def By_Property(self, property_name:str, property_value: Any) -> "Filter":
		for feat in self._json["features"]:
			props = feat.get("properties", {})
			if not property_value in props.get(property_name, []):
				self._json["features"].pop(feat)

	def By_Geomotries_type(self, geometry: List[str]):
		for feat in self._json["features"]:
			if not feat.get("geometry", {}).get("type","") in geometry:
				self._json["features"].pop(feat)
	
	def Get(self):
		return self._json