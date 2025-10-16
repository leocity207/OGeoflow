import math
from typing import Optional, TYPE_CHECKING

from point import Point


class Line:
	"""Class representing a 2-dimensional infinite line.
	Defined by a point and an angle (in radians).
	"""

	def __init__(self, point: "Point", angle: float):
		self.point = point
		self.angle = angle

	def get_point_on_line(self, distance: float = 1) -> "Point":
		"""Returns a point located `distance` units away along the line direction."""
		return self.point.get_new_point(distance, self.angle)

	def get_ABC(self) -> tuple[float, float, float]:
		"""Returns (A, B, C) coefficients for the line in the form Ax + By = C."""
		x2, y2 = self.get_point_on_line().xy()
		A = y2 - self.point.y
		B = self.point.x - x2
		C = A * self.point.x + B * self.point.y
		return A, B, C

	def intersects_line(self, line: "Line") -> Optional["Point"]:
		"""Computes the intersection point between two lines.
		Returns a Point if they intersect, or None if lines are parallel.
		"""

		A1, B1, C1 = self.get_ABC()
		A2, B2, C2 = line.get_ABC()

		det = A1 * B2 - A2 * B1
		if det == 0:
			# Lines are parallel, no intersection
			return None

		x = (B2 * C1 - B1 * C2) / det
		y = (A1 * C2 - A2 * C1) / det

		# Handle negative zero cases
		x = 0 if math.isclose(x, 0.0, abs_tol=1e-12) else x
		y = 0 if math.isclose(y, 0.0, abs_tol=1e-12) else y

		return Point(round(x, 10), round(y, 10))

	def __repr__(self):
		return f"Line(point={self.point}, angle={self.angle:.5f})"
