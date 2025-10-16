from enum import Enum
from typing import List, Optional
from math import tan, pi, sqrt

from DCEL.half_edge import HalfEdge, InflectionType
from topology.point import Point
from topology.line import Line
from topology.line_segment import LineSegment
from topology.polygon import Polygon
from topology.vector2D import Vector2D
from topology.ring import Ring
from configuration import Configuration, OuterEdge


class ContractionType(Enum):
    POSITIVE = "positive"
    NEGATIVE = "negative"


class Contraction:
    def __init__(self, configuration: Configuration, contraction_type: ContractionType, point: Point):
        self.type = contraction_type
        self.configuration = configuration
        self.point = point
        self.area_points = self.get_area_points()
        self.area = self.get_area()
        self.blocking_number = self.initialize_blocking_number()

    # ---------- Static Factory ----------
    @staticmethod
    def initialize(configuration: Configuration, contraction_type: ContractionType) -> Optional["Contraction"]:
        point = Contraction.get_point(configuration, contraction_type)
        return Contraction(configuration, contraction_type, point) if point else None

    # ---------- Validation ----------
    def is_feasible(self) -> bool:
        if not self.point:
            return False
        return self.area == 0 or (self.area > 0 and self.blocking_number == 0)

    def is_complementary(self, other: "Contraction") -> bool:
        return self.type != other.type

    def get_overlapping_edges(self, other: "Contraction") -> List[HalfEdge]:
        return [edge for edge in self.configuration.get_X() if edge in other.configuration.get_X()]

    def is_conflicting(self, complementary: "Contraction") -> bool:
        overlapping_edges = self.get_overlapping_edges(complementary)
        contraction_X = self.configuration.get_X()
        complementary_X = complementary.configuration.get_X()
        outer_edges = [contraction_X[0], contraction_X[2], complementary_X[0], complementary_X[2]]

        if not overlapping_edges:
            return False
        if len(overlapping_edges) == 1 and any(
            edge.get_inflection_type() == InflectionType.B for edge in outer_edges
        ):
            return False
        return True

    # ---------- Geometry: Point Selection ----------
    @staticmethod
    def get_point(configuration: Configuration, contraction_type: ContractionType) -> Optional[Point]:
        point_candidates = []

        inner_edge_normal = configuration.inner_edge.get_vector().get_normal().get_unit_vector()
        A = configuration.inner_edge.prev.tail.to_point() if configuration.inner_edge.prev else None
        D = configuration.inner_edge.next.get_head().to_point() if configuration.inner_edge.next else None
        track_prev = configuration.get_track(OuterEdge.PREV)
        track_next = configuration.get_track(OuterEdge.NEXT)

        if not (inner_edge_normal and A and D and track_prev and track_next):
            return None

        if configuration.inner_edge.get_inflection_type() == InflectionType.B:
            T = track_prev.intersects_line(track_next)
            if T:
                dist_T = Vector2D(configuration.inner_edge.tail.x - T.x,
                                  configuration.inner_edge.tail.y - T.y).dot(inner_edge_normal)
                point_candidates.append((T, dist_T))

        dist_A = configuration.inner_edge.prev.get_vector().dot(inner_edge_normal) if configuration.inner_edge.prev else None
        if dist_A is not None:
            point_candidates.append((A, dist_A))

        dist_D = configuration.inner_edge.next.twin.get_vector().dot(inner_edge_normal) if configuration.inner_edge.next and configuration.inner_edge.next.twin else None
        if dist_D is not None:
            point_candidates.append((D, dist_D))

        # Sort by distance from the inner edge
        point_candidates.sort(key=lambda x: x[1])

        if contraction_type == ContractionType.POSITIVE:
            for p, dist in point_candidates:
                if dist >= 0:
                    return p
        else:
            for p, dist in reversed(point_candidates):
                if dist <= 0:
                    return p
        return None

    # ---------- Area and Geometry ----------
    def get_area_points(self) -> List[Point]:
        c = self.configuration
        prev = c.get_outer_edge(OuterEdge.PREV)
        next_edge = c.get_outer_edge(OuterEdge.NEXT)

        if not (prev and next_edge and c.inner_edge):
            return []

        prev_head = prev.get_head()
        next_head = next_edge.get_head()
        inner_edge_head = c.inner_edge.get_head()
        inner_edge_angle = c.inner_edge.get_angle()

        if not (prev_head and next_head and inner_edge_head and isinstance(inner_edge_angle, (int, float))):
            return []

        outer_edge_prev_segment = LineSegment(prev.tail, prev_head)
        inner_edge_line = Line(self.point, inner_edge_angle)
        area_points = []

        if self.point.is_on_line_segment(outer_edge_prev_segment):
            area_points = [self.point, c.inner_edge.tail.to_point(), inner_edge_head.to_point()]
            if self.point.equals(prev.tail):
                point = c.get_track(OuterEdge.NEXT).intersects_line(inner_edge_line)
                if point:
                    area_points.append(point)
        else:
            area_points = [self.point, inner_edge_head.to_point(), c.inner_edge.tail.to_point()]
            if self.point.equals(next_head):
                point = c.get_track(OuterEdge.PREV).intersects_line(inner_edge_line)
                if point:
                    area_points.append(point)

        return area_points

    def get_area(self) -> float:
        if not self.area_points:
            return 0.0
        coords = [(p.x, p.y) for p in self.area_points]
        return Polygon.from_coordinates([coords]).area

    # ---------- Blocking Logic ----------
    def is_blocked_by(self, edge: HalfEdge) -> Optional[bool]:
        x = self.configuration.get_X()
        x_twin = self.configuration.inner_edge.twin.configuration.get_X() if self.configuration.inner_edge.twin and self.configuration.inner_edge.twin.configuration else []
        x.extend(x_twin)

        if edge in x:
            return False

        edge_line = edge.to_line_segment()
        if not edge_line:
            return None

        area_poly = Polygon([Ring(self.area_points)])
        points_in_polygon = [v for v in edge.get_endpoints() if v.is_in_polygon(area_poly)]
        intersections = area_poly.get_intersections(edge)

        x_line_segments = [e.to_line_segment() for e in x if e.to_line_segment() is not None]

        if len(points_in_polygon) == 2 or (
            intersections and any(not inter.is_on_line_segments(x_line_segments) for inter in intersections)
        ):
            return True
        return False

    def initialize_blocking_number(self) -> int:
        blocking_number = 0
        if not self.point:
            return blocking_number

        for boundary_edge in self.configuration.get_X_():
            if self.is_blocked_by(boundary_edge):
                blocking_number += 1
        return blocking_number

    # ---------- Blocking number updates ----------
    def decrement_blocking_number(self, x1x2: List[HalfEdge]):
        if self.blocking_number == 0:
            return
        decrement = sum(1 for e in x1x2 if self.is_blocked_by(e))
        self.blocking_number -= decrement

    def increment_blocking_number(self, x1x2: List[HalfEdge]):
        increment = sum(1 for e in x1x2 if self.is_blocked_by(e))
        self.blocking_number += increment

    # ---------- Compensation ----------
    def get_compensation_height(self, contraction_area: float) -> Optional[float]:
        a = self.configuration.inner_edge
        a_length = a.get_length()
        if not a.face or not a_length:
            return None

        alpha1 = a.tail.get_exterior_angle(a.face)
        alpha2 = a.get_head().get_exterior_angle(a.face) if a.get_head() else None
        if alpha1 is None or alpha2 is None:
            return None

        alpha1_ = -abs(alpha1) + pi * 0.5
        alpha2_ = -abs(alpha2) + pi * 0.5

        if alpha1_ == 0 and alpha2_ == 0:
            return contraction_area / a_length
        else:
            p = (a_length * 2) / (tan(alpha1_) + tan(alpha2_))
            q = (-contraction_area * 2) / (tan(alpha1_) + tan(alpha2_))
            return -0.5 * p + sqrt((0.5 * p) ** 2 - q)
