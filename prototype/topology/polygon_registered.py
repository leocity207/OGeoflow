from typing import List, Dict, Any
from line_string import Line_String


class Polygon:
	"""Represents one polygon with its boundary lines."""

	_feature: Dict[str, Any] 
	_coordinate: List 
	_polygon_id: int
	_linstring: List[Line_String]

	def __init__(self, feature: Dict[str, Any], coordinate: List, polygon_id: int):
		self._feature = feature
		self._coordinate = coordinate
		self._polygon_id = polygon_id

	def Get_Coordinates(self):
		return self._coordinate[0]
	
	def Get_Extrusions(self):
		return self._coordinate[1,:]

	def Register_Linestring(self, linestring: Line_String):
		self._linstring.append(linestring)

	def Get_Lines(self):
		return self._linstring
	
	def Reconstruct_From_Linestring() -> "Polygon":
		pass
	
	def __repr__(self):
		return f"Polygon(id={self._polygon_id}, coordintates={len(self._coordinate)})"