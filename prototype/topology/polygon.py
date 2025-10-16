from typing import List, Optional, TYPE_CHECKING


from point import Point
from dcel import HalfEdge
from line_segment import LineSegment
from ring import Ring


def crawl_array(array: list, idx: int, step: int = 1):
	"""Helper function equivalent to the JS crawlArray.
	Returns the element at (idx + step) modulo len(array).
	"""
	return array[(idx + step) % len(array)]


class Polygon:
	"""Class representing a 2-dimensional polygon, defined by one exterior ring and optional interior rings (holes)."""

	def __init__(self, rings: List["Ring"]):
		self.rings = rings

	@property
	def area(self) -> float:
		"""Calculates the signed area of the polygon (subtracting holes)."""
		total_area = 0.0

		for idx, ring in enumerate(self.rings):
			ring_area = 0.0
			n = len(ring.points)

			for i in range(n):
				add_x = ring.points[i].x
				add_y = ring.points[(i + 1) % n].y
				sub_x = ring.points[(i + 1) % n].x
				sub_y = ring.points[i].y

				ring_area += add_x * add_y * 0.5
				ring_area -= sub_x * sub_y * 0.5

			# Subtract area for holes
			total_area += abs(ring_area) * (-1 if idx > 0 else 1)

		return total_area

	@property
	def exterior_ring(self) -> "Ring":
		"""Returns the exterior ring of the polygon (outer boundary)."""
		return self.rings[0]

	@property
	def interior_rings(self) -> List["Ring"]:
		"""Returns all interior rings (holes)."""
		return self.rings[1:]

	@property
	def exterior_line_segments(self) -> List["LineSegment"]:
		"""Returns a list of line segments forming the exterior ring."""
		segments = []
		for idx, p in enumerate(self.exterior_ring.points):
			next_point = crawl_array(self.exterior_ring.points, idx, +1)
			segments.append(LineSegment(p, next_point))
		return segments

	def get_intersections(self, edge: "HalfEdge") -> Optional[List["Point"]]:
		"""Checks for intersections between the polygon’s exterior boundary and a given HalfEdge."""
		intersections: List["Point"] = []

		for boundary_edge in self.exterior_line_segments:
			segment = edge.to_line_segment()
			if segment:
				intersection = segment.intersects_line_segment(boundary_edge)
				if intersection and all(not p.equals(intersection) for p in intersections):
					intersections.append(intersection)

		return intersections if intersections else None

	@staticmethod
	def from_coordinates(coordinates: List[List[List[float]]]) -> "Polygon":
		"""Creates a Polygon from raw coordinate arrays [[ [x, y], [x, y], ... ], ...]."""

		rings = [Ring([Point(x, y) for x, y in ring]) for ring in coordinates]
		return Polygon(rings)

	def __repr__(self):
		return f"Polygon(num_rings={len(self.rings)}, area={self.area:.5f})"