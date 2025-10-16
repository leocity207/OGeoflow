"""
decompose_shared_borders.py

Backbone to extract shared and unique boundary LineStrings from a list of Polygone objects.

Assumptions about Polygone objects (as you described):
- poly.Get_Coordinates() -> returns outer ring coordinates as list of [ [x,y], ... ] (or similar)
- poly.Get_Extrusions() -> returns list of hole coordinate lists (each is list of [x,y]... ), may be empty
- poly.Register_Shared_Linestring(linestring_object) -> called to register shared edges

The script returns:
- line_records: list of dicts { 'linestring': shapely.geometry.LineString, 'polygons': [poly_ids], 'shared': bool }
- polygon_to_lines: mapping poly_id -> list of line_record ids
"""

from shapely.geometry import Polygon, MultiPolygon, LineString, GeometryCollection
from shapely.ops import linemerge, unary_union, snap
from shapely.strtree import STRtree
import math
import itertools
import uuid

# Utility: default epsilon to use if not provided
DEFAULT_EPSILON = 1e-8

def make_shapely_polygon_from_obj(poly_obj):
    """Convert your Polygone object into a shapely Polygon.
    Expects poly_obj.Get_Coordinates() -> outer ring list
            poly_obj.Get_Extrusions() -> list of holes (each a list)
    """
    outer = poly_obj.Get_Coordinates()
    holes = poly_obj.Get_Extrusions() or []
    # Ensure coordinates are in right format (tuples)
    outer_coords = [tuple(pt) for pt in outer]
    hole_coords = [[tuple(pt) for pt in hole] for hole in holes if hole]
    return Polygon(outer_coords, holes=hole_coords)

def _is_linestringish(geom):
    return geom.geom_type in ("LineString", "MultiLineString")

def _extract_linear_geoms(geom):
    """From a geometry (possibly GeometryCollection), yield LineStrings."""
    if geom is None:
        return []
    linear = []
    if geom.geom_type == "LineString":
        linear.append(geom)
    elif geom.geom_type == "MultiLineString":
        linear.extend(list(geom.geoms))
    elif geom.geom_type == "GeometryCollection":
        for g in geom.geoms:
            if g.geom_type in ("LineString", "MultiLineString"):
                if g.geom_type == "LineString":
                    linear.append(g)
                else:
                    linear.extend(list(g.geoms))
    return linear

def canonicalize_linestring(ls, epsilon):
    """Round coordinates to grid of epsilon and return normalized LineString with consistent orientation.
       Also remove extremely short segments and collapse if zero-length.
    """
    coords = [(round(x / epsilon) * epsilon, round(y / epsilon) * epsilon) for (x, y) in ls.coords]
    # remove consecutive duplicates
    new_coords = [coords[0]]
    for c in coords[1:]:
        if c != new_coords[-1]:
            new_coords.append(c)
    if len(new_coords) < 2:
        return None
    # ensure deterministic orientation: order coordinates lexicographically min-first
    if tuple(new_coords) > tuple(new_coords[::-1]):
        new_coords = new_coords[::-1]
    return LineString(new_coords)

def extract_shared_and_unique_lines(polygons, epsilon=DEFAULT_EPSILON, use_spatial_index=True):
    """
    polygons: list of Polygone objects (with methods you described)
    epsilon: tolerance for snapping / length thresholds / rounding.
    Returns:
       line_records: list of dicts { 'id', 'linestring', 'polygons': [poly_ids], 'shared': bool }
       polygon_to_lines: dict poly_id -> list of line_record ids
    Side-effects:
       calls poly.Register_Shared_Linestring(line_wrapper) for shared lines (2+ polygons)
    """
    # Build shapely geometries and id mapping
    poly_items = []  # list of (poly_id, poly_obj, shapely_geom)
    for i, p in enumerate(polygons):
        try:
            geom = make_shapely_polygon_from_obj(p)
        except Exception as e:
            raise ValueError(f"Cannot build shapely polygon for polygon index {i}: {e}")
        poly_id = getattr(p, "id", None) or getattr(p, "uid", None) or f"poly_{i}"
        poly_items.append((poly_id, p, geom))

    # Optional: snap small gaps before comparisons to unify near-adjacent vertices
    # We'll snap each polygon boundary to the union of all boundaries to avoid tiny slivers.
    boundaries = [item[2].boundary for item in poly_items]
    union_boundaries = unary_union(boundaries)
    snapped_boundaries = []
    for b in boundaries:
        snapped = snap(b, union_boundaries, epsilon)
        snapped_boundaries.append(snapped)

    # Replace poly_items geometries' boundaries with snapped version by constructing polygons with same coords
    # (We won't change polygon geometry interiors; we just use snapped boundaries for intersection operations.)
    # For robust handling, we'll create a copy of polygon geometries with snapped exterior and snapped interiors
    shapely_polys = []
    for (poly_id, poly_obj, geom), snapped_boundary in zip(poly_items, snapped_boundaries):
        # snapped_boundary might be MultiLineString or LineString. We need to turn them back into polygon(s)
        # A robust approach: keep original polygon geom but use snapped_boundary for pairwise boundary intersection computations.
        # We'll therefore store both original geom and snapped boundary.
        shapely_polys.append({
            "id": poly_id,
            "obj": poly_obj,
            "geom": geom,
            "boundary_for_comp": snapped_boundary
        })

    # Build spatial index of polygon envelopes to limit pairwise checks
    tree = None
    if use_spatial_index:
        tree = STRtree([entry["geom"] for entry in shapely_polys])
        # map geometry -> index
        geom_to_index = {entry["geom"]: idx for idx, entry in enumerate(shapely_polys)}
    else:
        tree = None

    # We'll collect detected shared lines into a dict keyed by canonical coords to deduplicate
    shared_lines_dict = {}  # key -> {'linestring': LineString, 'polygons': set(poly_id)}
    def add_shared_line(ls, poly_id_a, poly_id_b):
        can = canonicalize_linestring(ls, epsilon)
        if can is None:
            return
        key = tuple(can.coords)
        rec = shared_lines_dict.get(key)
        if rec is None:
            shared_lines_dict[key] = {'linestring': can, 'polygons': set([poly_id_a, poly_id_b])}
        else:
            rec['polygons'].update([poly_id_a, poly_id_b])

    # Pairwise intersection loop (spatially constrained)
    n = len(shapely_polys)
    for i, entry in enumerate(shapely_polys):
        poly_i = entry["geom"]
        b_i = entry["boundary_for_comp"]
        poly_id_i = entry["id"]
        # candidate indices from tree
        candidates = range(n) if tree is None else [geom_to_index[g] for g in tree.query(poly_i)]
        for j in candidates:
            if j <= i:
                continue
            entry_j = shapely_polys[j]
            poly_j = entry_j["geom"]
            poly_id_j = entry_j["id"]
            # quick envelope check
            if not poly_i.envelope.intersects(poly_j.envelope):
                continue
            # compute intersection of boundaries (we only want boundary-boundary overlaps)
            b_j = entry_j["boundary_for_comp"]
            inter = b_i.intersection(b_j)
            # extract linear components
            linear_parts = _extract_linear_geoms(inter)
            for part in linear_parts:
                if part.length <= epsilon:
                    continue
                # possibility: intersection may yield MultiLineString with overlapping small pieces; split into pieces and canonicalize
                # add as shared
                add_shared_line(part, poly_id_i, poly_id_j)

    # After all pairs processed, create a list of shared lines and register them with polygon objects
    line_records = []
    polygon_to_lines = {entry["id"]: [] for entry in shapely_polys}
    for key, rec in shared_lines_dict.items():
        ls = rec['linestring']
        poly_ids = sorted(list(rec['polygons']))
        rec_id = str(uuid.uuid4())
        line_record = {'id': rec_id, 'linestring': ls, 'polygons': poly_ids, 'shared': True}
        line_records.append(line_record)
        for pid in poly_ids:
            polygon_to_lines[pid].append(rec_id)
        # Call Register_Shared_Linestring on polygon objects (if they implement it)
        for entry in shapely_polys:
            if entry["id"] in poly_ids:
                try:
                    entry["obj"].Register_Shared_Linestring(ls)
                except Exception:
                    # ignore if user object lacks that function or it raises
                    pass

    # Build union of all shared lines for subtraction
    if line_records:
        union_shared = unary_union([rec['linestring'] for rec in line_records])
    else:
        union_shared = None

    # For each polygon, compute remaining unique boundary parts by difference
    for entry in shapely_polys:
        pid = entry["id"]
        b = entry["geom"].boundary
        if union_shared is not None:
            unique = b.difference(union_shared)
        else:
            unique = b
        # from unique geometry extract linear parts, canonicalize and add as unique line records
        linear_unique = _extract_linear_geoms(unique)
        # Merge contiguous pieces to have nicer long lines
        if linear_unique:
            merged = linemerge(unary_union(linear_unique))
            # merged may be LineString or MultiLineString
            merged_list = []
            if merged.geom_type == "LineString":
                merged_list = [merged]
            elif merged.geom_type == "MultiLineString":
                merged_list = list(merged.geoms)
            else:
                # possibly a GeometryCollection, fall back to linear_unique
                merged_list = linear_unique
        else:
            merged_list = []

        for part in merged_list:
            if part.length <= epsilon:
                continue
            can = canonicalize_linestring(part, epsilon)
            if can is None:
                continue
            # avoid duplicating with shared lines (possible due to rounding); ensure it's not equal to any shared record
            coords_key = tuple(can.coords)
            if coords_key in shared_lines_dict:
                continue
            rec_id = str(uuid.uuid4())
            rec = {'id': rec_id, 'linestring': can, 'polygons': [pid], 'shared': False}
            line_records.append(rec)
            polygon_to_lines[pid].append(rec_id)

    # Optionally we can attempt to merge contiguous collinear boundary pieces across all lines (linemerge)
    # but that can re-associate segments—skip for backbone; user can call linemerge later if desired.

    return line_records, polygon_to_lines

# Example usage (adapt to your code)
if __name__ == "__main__":
    # Example stub polygon class to test algorithm (replace with your actual objects)
    class StubPoly:
        def __init__(self, coords, holes=None, id=None):
            self._coords = coords
            self._holes = holes or []
            self.id = id
            self.shared = []
        def Get_Coordinates(self):
            return self._coords
        def Get_Extrusions(self):
            return self._holes
        def Register_Shared_Linestring(self, ls):
            # store the linestring for demo
            self.shared.append(ls)

    # minimal test: two adjacent squares sharing a border
    p1 = StubPoly([[0,0],[1,0],[1,1],[0,1],[0,0]], id="A")
    p2 = StubPoly([[1,0],[2,0],[2,1],[1,1],[1,0]], id="B")
    polygons = [p1, p2]

    recs, mapping = extract_shared_and_unique_lines(polygons, epsilon=1e-9)
    print("Line records:")
    for r in recs:
        print(r['id'], "shared" if r['shared'] else "unique", r['polygons'], list(r['linestring'].coords))
    print("Polygon->lines mapping:", mapping)
    print("Registered shared lines on objects:", [(p.id, len(p.shared)) for p in polygons])
