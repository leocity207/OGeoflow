import math
from typing import List, Tuple, TYPE_CHECKING


from line_segment import LineSegment
from polygon import Polygon
from vector2D import Vector2D


class Point:
    """Class representing a 2-dimensional point."""

    def __init__(self, x: float, y: float):
        """Create a point."""
        self.x = x
        self.y = y

    def xy(self) -> Tuple[float, float]:
        """Gets the x and y coordinates of the point as a tuple."""
        return self.x, self.y

    def to_vector(self) -> "Vector2D":
        """Converts the point into a Vector2D."""
        return Vector2D(self.x, self.y)

    def distance_to_point(self, p: "Point") -> float:
        """Calculates Euclidean distance to another Point."""
        x1, y1 = self.xy()
        x2, y2 = p.xy()
        a = x1 - x2
        b = y1 - y2
        return math.sqrt(a * a + b * b)

    def distance_to_line_segment(self, l: "LineSegment") -> float:
        """Computes the shortest distance from this point to a line segment."""
        vx, vy = self.xy()
        e1x, e1y = l.endPoint1.xy()
        e2x, e2y = l.endPoint2.xy()

        edx = e2x - e1x
        edy = e2y - e1y
        line_length_sq = edx ** 2 + edy ** 2

        if line_length_sq == 0:
            return math.sqrt((vx - e1x) ** 2 + (vy - e1y) ** 2)

        t = ((vx - e1x) * edx + (vy - e1y) * edy) / line_length_sq
        t = max(0, min(1, t))

        ex = e1x + t * edx
        ey = e1y + t * edy
        dx = vx - ex
        dy = vy - ey

        return math.sqrt(dx * dx + dy * dy)

    def get_new_point(self, distance: float, angle: float) -> "Point":
        """Returns a new Point offset by `distance` in the direction of `angle` (radians)."""
        dx = 0 if angle in (math.pi * 0.5, math.pi * 1.5) else math.cos(angle)
        dy = 0 if angle in (math.pi * 1, math.pi * 2) else math.sin(angle)
        return Point(self.x + distance * dx, self.y + distance * dy)

    def is_on_line_segment(self, line_segment: "LineSegment") -> bool:
        """Checks if the point lies exactly on a line segment."""
        pa = self.distance_to_point(line_segment.endPoint1)
        pb = self.distance_to_point(line_segment.endPoint2)
        return round(pa + pb, 10) == round(line_segment.length, 10)

    def is_on_line_segments(self, line_segments: List["LineSegment"]) -> bool:
        """Checks if the point lies on any of the given line segments."""
        return any(self.is_on_line_segment(ls) for ls in line_segments)

    def is_in_polygon(self, polygon: "Polygon") -> bool:
        """
        Determines whether the point lies within a convex polygon.
        Algorithm: sign of cross product remains constant for convex polygons.
        """
        A, B, C = [], [], []

        points = polygon.exteriorRing.points
        for idx, p1 in enumerate(points):
            p2 = points[(idx + 1) % len(points)]
            a = -(p2.y - p1.y)
            b = p2.x - p1.x
            c = -(a * p1.x + b * p1.y)
            A.append(a)
            B.append(b)
            C.append(round(c, 10))

        D = [round(a * self.x + b * self.y + c, 10) for a, b, c in zip(A, B, C)]
        t1 = all(d >= 0 for d in D)
        t2 = all(d <= 0 for d in D)
        return t1 or t2

    def equals(self, point: "Point") -> bool:
        """Checks if this point is equal to another point (within 1e-10 tolerance)."""
        return round(self.x - point.x, 10) == 0 and round(self.y - point.y, 10) == 0

    def __repr__(self):
        return f"Point(x={self.x}, y={self.y})"
