from typing import List, Optional, Dict, Any, TYPE_CHECKING

from polygon import Polygon


class MultiPolygon:
    """
    Class representing a 2-dimensional MultiPolygon.
    It is defined by its constituent Polygon objects.
    """

    def __init__(
        self,
        polygons: List["Polygon"],
        id: Optional[str] = None,
        properties: Optional[Dict[str, Any]] = None,
    ):
        self.polygons = polygons
        self.id = id
        self.properties = properties

    @property
    def area(self) -> float:
        """Returns the total area of all polygons in the MultiPolygon."""
        return sum(polygon.area for polygon in self.polygons)

    @staticmethod
    def from_coordinates(coordinates: List[List[List[List[float]]]]) -> "MultiPolygon":
        """
        Create a MultiPolygon from GeoJSON-style coordinates.

        Args:
            coordinates: A nested list in the form [[[ [x, y], ... ], ...], ...],
                         where each top-level element represents a polygon
                         composed of one or more rings.

        Returns:
            A MultiPolygon instance.
        """
        polygons = [Polygon.from_coordinates(polygon_coords) for polygon_coords in coordinates]
        return MultiPolygon(polygons)

    def __repr__(self):
        return f"MultiPolygon({len(self.polygons)} polygons, id={self.id})"
