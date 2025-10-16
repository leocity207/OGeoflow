from typing import List, Optional, TYPE_CHECKING

if TYPE_CHECKING:
	from point import Point
	from polygon import Polygon
	from vector2D import Vector2D


class LineSegment:
	"""Class representing a 2D finite line segment defined by its endpoints."""

	def __init__(self, end_point1: "Point", end_point2: "Point"):
		self.end_point1 = end_point1
		self.end_point2 = end_point2

	@property
	def length(self) -> float:
		"""Returns the Euclidean length of the line segment."""
		return self.end_point1.distance_to_point(self.end_point2)

	def intersects_line_segment(
		self,
		other: "LineSegment",
		consider_collinear_overlap: bool = False
	) -> Optional["Point"]:
		"""
		Determines whether this line segment intersects another line segment.

		Adapted from:
		https://www.codeproject.com/tips/862988/find-the-intersection-point-of-two-line-segments

		Args:
			other: Another LineSegment instance.
			consider_collinear_overlap: If True, returns a NaN point when overlapping.

		Returns:
			A Point of intersection if found, or None otherwise.
		"""

		p1 = Vector2D(self.end_point1.x, self.end_point1.y)
		p2 = Vector2D(self.end_point2.x, self.end_point2.y)
		q1 = Vector2D(other.end_point1.x, other.end_point1.y)
		q2 = Vector2D(other.end_point2.x, other.end_point2.y)

		r = p2.minus(p1)
		s = q2.minus(q1)
		rxs = r.cross(s)
		q1p1xr = q1.minus(p1).cross(r)

		# Case 1: Collinear
		if rxs == 0 and q1p1xr == 0:
			if consider_collinear_overlap:
				if (
					0 <= q1.minus(p1).dot(r) <= r.dot(r)
					or 0 <= p1.minus(q1).dot(s) <= s.dot(s)
				):
					return Point(float("nan"), float("nan"))
			# Collinear but disjoint
			return None

		# Case 2: Parallel and non-intersecting
		if rxs == 0 and q1p1xr != 0:
			return None

		# Compute intersection parameters
		t = q1.minus(p1).cross(s) / rxs
		u = q1.minus(p1).cross(r) / rxs

		# Case 3: Proper intersection
		if rxs != 0 and 0 <= t <= 1 and 0 <= u <= 1:
			intersection_v = p1.plus(r.times(t))
			return Point(intersection_v.dx, intersection_v.dy)

		# Case 4: Non-parallel, but segments don't intersect
		return None

	def intersects_polygon(self, polygon: "Polygon") -> List["Point"]:
		"""
		Checks if this line segment intersects with a given Polygon.
		Only considers the exterior line segments of the polygon.

		Args:
			polygon: The Polygon to test for intersection.

		Returns:
			A list of intersection Points.
		"""
		intersections = []
		for edge in polygon.exterior_line_segments:
			intersection = self.intersects_line_segment(edge)
			if intersection:
				intersections.append(intersection)
		return intersections

	def __repr__(self):
		return f"LineSegment({self.end_point1}, {self.end_point2})"
