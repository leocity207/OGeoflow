from typing import Any

class Line_String:
	"""Represents a unique border line between polygons."""

	_coordinate: Any

	def __init__(self, coordinates: Any):
		self._coordinate = coordinates
		

	def __repr__(self):
		return f"Line(coordinates={len(self._coordinate)})"