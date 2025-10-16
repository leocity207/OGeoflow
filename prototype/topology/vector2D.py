import math
from typing import Tuple, TYPE_CHECKING


from point import Point


class Vector2D:
    """Class representing a 2-dimensional vector."""

    def __init__(self, dx: float, dy: float):
        self.dx = dx
        self.dy = dy

    def get_magnitude(self) -> float:
        """Returns the vector's magnitude (Euclidean length)."""
        return math.sqrt(self.dx ** 2 + self.dy ** 2)

    def get_normal(self, counterclockwise: bool = True) -> "Vector2D":
        """Returns the vector's normal (perpendicular) vector.
        If counterclockwise=True, returns the left-hand normal.
        """
        if counterclockwise:
            return Vector2D(-self.dy, self.dx)
        else:
            return Vector2D(self.dy, -self.dx)

    def get_inverse(self) -> "Vector2D":
        """Returns the inverse (opposite) vector."""
        return Vector2D(-self.dx, -self.dy)

    def get_unit_vector(self) -> "Vector2D":
        """Returns a normalized version of the vector (unit length)."""
        magnitude = self.get_magnitude()
        if magnitude == 0:
            raise ValueError("Cannot normalize a zero-length vector.")
        return self.times(1 / magnitude)

    def plus(self, vector: "Vector2D") -> "Vector2D":
        """Adds another vector to this one."""
        return Vector2D(self.dx + vector.dx, self.dy + vector.dy)

    def minus(self, vector: "Vector2D") -> "Vector2D":
        """Subtracts another vector from this one."""
        return Vector2D(self.dx - vector.dx, self.dy - vector.dy)

    def times(self, scalar: float) -> "Vector2D":
        """Scales the vector by a scalar."""
        return Vector2D(self.dx * scalar, self.dy * scalar)

    def dot(self, vector: "Vector2D") -> float:
        """Returns the dot product with another vector."""
        return self.dx * vector.dx + self.dy * vector.dy

    def cross(self, vector: "Vector2D") -> float:
        """Returns the 2D cross product (scalar z-component)."""
        return self.dx * vector.dy - self.dy * vector.dx

    def to_point(self) -> "Point":
        """Converts this vector into a Point (dx, dy)."""
        return Point(self.dx, self.dy)

    def to_array(self) -> Tuple[float, float]:
        """Returns the vector components as a tuple (dx, dy)."""
        return self.dx, self.dy

    def __repr__(self):
        return f"Vector2D(dx={self.dx}, dy={self.dy})"
