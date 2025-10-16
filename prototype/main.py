import json
from shapely.geometry import shape, Polygon, LineString, MultiPolygon
from shapely.ops import unary_union
from collections import defaultdict

class LineUnit:
    """Represents a unique border line between polygons."""
    def __init__(self, geom: LineString):
        self.geom = geom
        self.polygons = []  # PolygonUnit references that share this line

    def add_polygon(self, poly):
        if poly not in self.polygons:
            self.polygons.append(poly)

    def key(self):
        # Normalized key for geometric equality (order independent)
        coords = list(self.geom.coords)
        return tuple(sorted([coords[0], coords[-1]]))

    def __repr__(self):
        return f"LineUnit(len={len(self.geom.coords)}, shared_by={len(self.polygons)})"


class PolygonUnit:
    """Represents one polygon with its boundary lines."""
    def __init__(self, geom: Polygon, poly_id: int, props=None):
        self.id = poly_id
        self.geom = geom
        self.props = props or {}
        self.lines = []  # list of LineUnit references

    def add_line(self, line: LineUnit):
        if line not in self.lines:
            self.lines.append(line)

    def __repr__(self):
        return f"PolygonUnit(id={self.id}, n_lines={len(self.lines)})"


def build_topology(polygon_units):
    """
    Build a shared-edge topology.
    For each polygon edge, detect if it’s shared by another polygon.
    """
    print("Building topology ...")
    line_map = {}

    for poly in polygon_units:
        # Extract outer boundary coordinates as segments
        coords = list(poly.geom.exterior.coords)
        for i in range(len(coords) - 1):
            seg = LineString([coords[i], coords[i + 1]])

            # Normalize to handle both directions
            key = tuple(sorted([coords[i], coords[i + 1]]))
            if key not in line_map:
                line_map[key] = LineUnit(seg)
            line_map[key].add_polygon(poly)
            poly.add_line(line_map[key])

    print(f"Created {len(line_map)} unique LineUnits.")

    # Optional: print sharing stats
    shared = sum(1 for l in line_map.values() if len(l.polygons) > 1)
    print(f"  Shared borders: {shared}")
    print(f"  Exterior borders: {len(line_map) - shared}")

    return list(line_map.values())


def load_polygons_from_geojson(geojson_path, dep_code="29"):
    """Load dep_code polygons from GeoJSON and split MultiPolygons."""
    with open(geojson_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    polygons = []
    poly_id = 0
    for feat in data["features"]:
        props = feat.get("properties", {})
        if not dep_code in props.get("dep_code", []):
            continue

        geom = shape(feat["geometry"])
        if isinstance(geom, Polygon):
            polygons.append(PolygonUnit(geom, poly_id, props))
            poly_id += 1
        elif isinstance(geom, MultiPolygon):
            for subgeom in geom.geoms:
                polygons.append(PolygonUnit(subgeom, poly_id, props))
                poly_id += 1

    print(f"Loaded {len(polygons)} PolygonUnits from dep_code={dep_code}")
    return polygons


if __name__ == "__main__":
    geojson_path = "filtered_dep29.geojson"

    # Step 1: load polygons (split MultiPolygon)
    polygons = load_polygons_from_geojson(geojson_path, dep_code="29")

    # Step 2: build topology
    lines = build_topology(polygons)

    # Step 3: quick summary
    print(f"\nExample polygon: {polygons[0]}")
    print(f"First 3 lines of that polygon:")
    for ln in polygons[0].lines[:3]:
        print("  ", ln)