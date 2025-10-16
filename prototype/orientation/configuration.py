from enum import Enum
from typing import List, Optional, TYPE_CHECKING

from geometry.Line import Line
from utilities import crawl_array
from .Contraction import Contraction, ContractionType

if TYPE_CHECKING:
    from DCEL.HalfEdge import HalfEdge
    from DCEL.Vertex import Vertex
    from geometry.Point import Point


class OuterEdge(Enum):
    PREV = "prev"
    NEXT = "next"


class Junction(Enum):
    A = "aligned"
    B = "unalignedDifferentSide"
    C = "unalignedSameSide"


class Configuration:
    """
    Represents a local geometric configuration consisting of a DCEL half-edge
    and its two neighboring outer edges (previous and next).
    """

    def __init__(self, edge: "HalfEdge"):
        self.inner_edge: "HalfEdge" = edge
        self.contractions = {
            ContractionType.N: Contraction.initialize(self, ContractionType.N),
            ContractionType.P: Contraction.initialize(self, ContractionType.P),
        }

    def get_outer_edge(self, position: OuterEdge) -> Optional["HalfEdge"]:
        """Return the outer (previous or next) edge relative to the inner edge."""
        if position == OuterEdge.PREV:
            return self.inner_edge.prev
        else:
            return self.inner_edge.next

    def get_X(self) -> List["HalfEdge"]:
        """
        Get all three edges forming the configuration: [prev, inner, next].
        Returns an empty list if any of them is missing.
        """
        prev_edge = self.get_outer_edge(OuterEdge.PREV)
        next_edge = self.get_outer_edge(OuterEdge.NEXT)
        return [prev_edge, self.inner_edge, next_edge] if prev_edge and next_edge else []

    def get_X_(self) -> List["HalfEdge"]:
        """
        Get all edges of the boundary except those that form the configuration.
        (Essentially the inverse of get_X.)
        """
        x = self.get_X()
        if not x:
            return []
        return [edge for edge in self.inner_edge.get_cycle() if edge not in x]

    def get_track(self, outer_edge: OuterEdge) -> Optional[Line]:
        """
        Compute the track line corresponding to a given outer edge of the configuration.
        Returns a Line object or None if insufficient data.
        """
        prev_edge = self.get_outer_edge(OuterEdge.PREV)
        next_edge = self.get_outer_edge(OuterEdge.NEXT)

        prev_angle = prev_edge.get_angle() if prev_edge else None
        next_angle = next_edge.get_angle() if next_edge else None
        head = self.inner_edge.get_head()

        if not prev_edge or not next_edge or prev_angle is None or next_angle is None or not head:
            return None

        if outer_edge == OuterEdge.PREV:
            return Line(self.inner_edge.tail, prev_angle)
        else:
            return Line(head, next_angle)

    @property
    def tracks(self) -> List[Optional[Line]]:
        """Return both track lines of the configuration."""
        return [self.get_track(OuterEdge.PREV), self.get_track(OuterEdge.NEXT)]

    @property
    def track_intersection(self) -> Optional["Point"]:
        """Compute the intersection point between the two configuration tracks."""
        prev_track, next_track = self.tracks
        if not prev_track or not next_track:
            return None
        return prev_track.intersects_line(next_track)

    def has_junction(self) -> bool:
        """
        Check if the configuration contains a junction vertex,
        i.e., a vertex connected to more than two edges.
        """
        return any(len(p.edges) > 2 for p in self.inner_edge.get_endpoints())

    def get_junction_type(self, vertex: "Vertex") -> Optional[Junction]:
        """
        Determine the type of junction at a given vertex.

        Returns:
            Junction.A if aligned,
            Junction.B if unaligned and on different sides,
            Junction.C if unaligned but on same side,
            or None if not applicable.
        """
        if not self.inner_edge.twin:
            return None

        try:
            idx = vertex.edges.index(self.inner_edge)
        except ValueError:
            try:
                idx = vertex.edges.index(self.inner_edge.twin)
            except ValueError:
                return None

        edge1 = crawl_array(vertex.edges, idx, +1)
        edge2 = crawl_array(vertex.edges, idx, +2)

        if edge1.get_angle() == (edge2.twin.get_angle() if edge2.twin else None):
            return Junction.A

        normal = self.inner_edge.get_vector().get_normal() if self.inner_edge.get_vector() else None
        if not normal:
            return None

        o1 = edge1.get_vector().dot(normal) if edge1.get_vector() else None
        o2 = edge2.get_vector().dot(normal) if edge2.get_vector() else None
        if o1 is None or o2 is None:
            return None

        if (o1 > 0 and o2 < 0) or (o1 < 0 and o2 > 0):
            return Junction.B
        else:
            return Junction.C

    def __repr__(self):
        return f"Configuration(inner_edge={self.inner_edge})"
