from typing import List, Tuple, TYPE_CHECKING


from point import Point


class Ring:
	"""Class representing a ring (closed linear ring of points).
	The ring's points are always returned in counterclockwise order.
	"""

	def __init__(self, points: List["Point"]):
		self._points = points

	@staticmethod
	def from_coordinates(coordinates: List[Tuple[float, float]]) -> "Ring":
		"""Creates a Ring from a list of coordinate tuples [(x, y), (x, y), ...]."""
		points = [Point(x, y) for x, y in coordinates]
		return Ring(points)

	@property
	def points(self) -> List["Point"]:
		"""Returns the points in counterclockwise order."""
		# Reverse the list if the ring is clockwise
		return list(reversed(self._points)) if self.is_clockwise else self._points

	@property
	def length(self) -> int:
		"""Returns the number of points in the ring."""
		return len(self.points)

	@property
	def is_clockwise(self) -> bool:
		"""Determines if the ring points are ordered clockwise."""
		total = 0.0
		n = len(self._points)
		for i, point in enumerate(self._points):
			next_point = self._points[(i + 1) % n]
			total += (point.x + next_point.x) * (point.y - next_point.y)
		return total > 0

	@property
	def centroid(self) -> Tuple[float, float]:
		"""Calculates and returns the centroid (geometric center) of the ring."""
		sum_x = 0.0
		sum_y = 0.0
		total_area = 0.0
		n = len(self.points)

		for i, point in enumerate(self.points):
			prev_point = self.points[(i - 1) % n]
			area_segment = point.x * prev_point.y - prev_point.x * point.y
			sum_x += (point.x + prev_point.x) * area_segment
			sum_y += (point.y + prev_point.y) * area_segment
			total_area += area_segment

		total_area *= 0.5
		if abs(total_area) < 1e-12:
			raise ValueError("Degenerate ring: zero area, cannot compute centroid.")

		centroid_x = sum_x / (6 * total_area)
		centroid_y = sum_y / (6 * total_area)
		return (centroid_x, centroid_y)

	def __repr__(self):
		return f"Ring(num_points={len(self._points)}, clockwise={self.is_clockwise})"
