#!/usr/bin/env python3
"""
plot_dcel_by_coords.py

Visualize DCEL and annotate using coordinate-based IDs (not Python object identity).

Key points:
 - Two half-edges are considered the same if their (tail,head) coordinates are equal
   up to a tolerance `eps` (CLI option --tol).
 - Faces are mapped to indices by using their representative half-edge (face.edge).
 - For each half-edge we display indices of next/prev/twin/face (global indices in storage.half_edges / storage.faces).
 - For each vertex we list outgoing half-edge indices (by coordinate matching) and, optionally, each outgoing's relations.
"""
from __future__ import annotations
import argparse
import math
import sys
from collections import defaultdict
from typing import Any, Dict, List, Tuple, Optional

import matplotlib.pyplot as plt
from matplotlib.patches import FancyArrowPatch

import pygeoflow
from pygeoflow import dcel


# ---------------- helpers: quantization & keys ----------------
def quant_key(x: float, eps: float) -> int:
	# quantize coordinate to integer grid with step eps
	return int(round(x / eps))


def coord_key_xy(x: float, y: float, eps: float) -> Tuple[int, int]:
	return (quant_key(x, eps), quant_key(y, eps))


def he_coord_key(tail_x: float, tail_y: float, head_x: float, head_y: float, eps: float):
	return (coord_key_xy(tail_x, tail_y, eps), coord_key_xy(head_x, head_y, eps))


def tofloat(v: Any) -> float:
	try:
		return float(v)
	except Exception:
		return float(v.x)


# ---------------- build maps ----------------
def build_vertex_coords_list(vertices_seq, eps: float):
	verts = [v for v in vertices_seq]
	coords = [(tofloat(v.x), tofloat(v.y)) for v in verts]
	qcoords = [coord_key_xy(x, y, eps) for (x, y) in coords]
	return verts, coords, qcoords  # lists aligned by index


def build_halfedge_coord_keys(half_edges, eps: float):
	hes = [he for he in half_edges]
	coords = []
	keys = []
	for he in hes:
		try:
			tx = tofloat(he.tail.x)
			ty = tofloat(he.tail.y)
			hx = tofloat(he.head.x)
			hy = tofloat(he.head.y)
		except Exception:
			# defensive fallback
			tx = ty = hx = hy = 0.0
		coords.append((tx, ty, hx, hy))
		keys.append(he_coord_key(tx, ty, hx, hy, eps))
	return hes, coords, keys


def build_halfedge_key_map(keys: List[Tuple[Tuple[int,int],Tuple[int,int]]]):
	# map key -> list of half-edge indices that share the same tail/head quantized coords
	m = defaultdict(list)
	for idx, k in enumerate(keys):
		m[k].append(idx)
	return m


def build_face_repr_map(faces_seq, halfedge_coords_keys_map, hes_coords_keys, eps: float):
	"""
	Map from representative half-edge index -> face index.
	We try to use face.edge (if provided) and find its corresponding half-edge index via coordinate key.
	"""
	face_map = {}  # halfedge_index -> face_index
	for fi, face in enumerate(faces_seq):
		try:
			fe = getattr(face, "edge", None)
			if fe is None:
				continue
			# get fe coords:
			tx = tofloat(fe.tail.x)
			ty = tofloat(fe.tail.y)
			hx = tofloat(fe.head.x)
			hy = tofloat(fe.head.y)
			key = he_coord_key(tx, ty, hx, hy, eps)
			candidates = halfedge_coords_keys_map.get(key, [])
			if candidates:
				# choose first candidate as representative
				face_map[candidates[0]] = fi
		except Exception:
			continue
	return face_map  # may be incomplete


# ---------------- resolve relations by coordinate matching ----------------
def resolve_he_index_by_object(obj_he, halfedge_coords_keys_map, eps: float):
	"""
	Given an object that should behave like a half-edge (has tail/head),
	return the best-matching half-edge index(es) by comparing quantized coords.
	Returns list of candidate indices (possibly empty).
	"""
	if obj_he is None:
		return []
	try:
		tx = tofloat(obj_he.tail.x)
		ty = tofloat(obj_he.tail.y)
		hx = tofloat(obj_he.head.x)
		hy = tofloat(obj_he.head.y)
	except Exception:
		return []
	key = he_coord_key(tx, ty, hx, hy, eps)
	return halfedge_coords_keys_map.get(key, [])


# ---------------- plotting utilities ----------------
def draw_arrow(ax, x0, y0, x1, y1, frac=0.0, color="k"):
	if math.isclose(x0, x1, abs_tol=1e-12) and math.isclose(y0, y1, abs_tol=1e-12):
		off = 0.02
		patch = FancyArrowPatch((x0, y0), (x0 + off, y0 + off),
								connectionstyle=f"arc3,rad={frac if frac != 0 else 0.3}",
								arrowstyle="->", mutation_scale=10.0, linewidth=0.8, color=color, zorder=2)
	else:
		patch = FancyArrowPatch((x0, y0), (x1, y1),
								connectionstyle=f"arc3,rad={frac}",
								arrowstyle="->", shrinkA=4, shrinkB=6,
								mutation_scale=10.0, linewidth=0.8, color=color, zorder=2)
	ax.add_patch(patch)


# ---------------- main plotting function ----------------
def plot_dcel_with_coord_ids(storage: Any,
							 ax: plt.Axes,
							 eps: float = 1e-9,
							 annotate_vertices: bool = True,
							 annotate_halfedges: bool = True,
							 annotate_vertex_relations: bool = True,
							 annotate_relations: bool = False,
							 curvature_frac: float = 0.2):
	verts_seq = list(storage.vertices)
	hes_seq = list(storage.half_edges)
	faces_seq = list(getattr(storage, "faces", []))

	verts, vert_coords, vert_qcoords = build_vertex_coords_list(verts_seq, eps)
	hes, he_coords, he_keys = build_halfedge_coord_keys(hes_seq, eps)
	he_key_map = build_halfedge_key_map(he_keys)
	face_repr_map = build_face_repr_map(faces_seq, he_key_map, he_keys, eps)

	# convenience: lists for plotting
	xs = [x for (x, y) in vert_coords]
	ys = [y for (x, y) in vert_coords]

	# scatter vertices
	ax.scatter(xs, ys, s=40, zorder=4, edgecolor="k", linewidth=0.4)
	if annotate_vertices:
		for vidx, (x, y) in enumerate(vert_coords):
			# outgoing half-edges = those whose tail quantized coord equals this vertex qcoord
			vq = vert_qcoords[vidx]
			outs = []
			# find half-edges whose tail qcoord == vq
			for he_idx, key in enumerate(he_keys):
				tail_key, head_key = key
				if tail_key == vq:
					outs.append(he_idx)
			# build label
			lines = [f"v:{vidx}"]
			if outs:
				lines.append("outs:" + "[" + ",".join(str(i) for i in outs) + "]")
				if annotate_vertices:
					for vidx, (x, y) in enumerate(vert_coords):
						vq = vert_qcoords[vidx]
						outs = []
						for he_idx, key in enumerate(he_keys):
							tail_key, _ = key
							if tail_key == vq:
								outs.append(he_idx)

						label = f"v:{vidx}"
						if outs:
							label += "\nouts:[" + ",".join(map(str, outs)) + "]"

						ax.text(
							x, y, label,
							fontsize=6,
							va="bottom",
							ha="right",
							bbox=dict(boxstyle="round,pad=0.12", fc="white", alpha=0.9, linewidth=0.25),
							zorder=6
						)

	# draw half-edges and annotate them with computed indices
	for he_idx, (tx, ty, hx, hy) in enumerate(he_coords):
		# draw arrow
		frac = curvature_frac  # we keep simple frac; user earlier wanted no change
		draw_arrow(ax, tx, ty, hx, hy, frac=frac, color="tab:gray")

		# annotate half-edge with id and resolved face index (coordinate-based)
		if annotate_halfedges:
			# midpoint
			mx, my = (tx + hx) / 2.0, (ty + hy) / 2.0
			dx, dy = hx - tx, hy - ty
			dist = math.hypot(dx, dy) 
			ux, uy = -dy / dist, dx / dist
			off = 0.10 * dist

			txpos = mx + ux * off
			typos = my + uy * off

			# resolve relations by coordinates
			def one_or_dash(cands):
				return cands[0] if cands else "-"

			n = one_or_dash(resolve_he_index_by_object(getattr(hes_seq[he_idx], "next", None), he_key_map, eps))
			p = one_or_dash(resolve_he_index_by_object(getattr(hes_seq[he_idx], "prev", None), he_key_map, eps))
			t = one_or_dash(resolve_he_index_by_object(getattr(hes_seq[he_idx], "twin", None), he_key_map, eps))

			# face resolution
			f_id = "-"
			try:
				fobj = hes_seq[he_idx].face
				if fobj is not None:
					fe = getattr(fobj, "edge", None)
					if fe is not None:
						cands = resolve_he_index_by_object(fe, he_key_map, eps)
						if cands and cands[0] in face_repr_map:
							f_id = face_repr_map[cands[0]]
			except Exception:
				pass

			label = (
				f"indx:{he_idx}\n"
				f"face:{f_id}\n"
				f"next:{n}\n"
				f"prev:{p}\n"
				f"twin:{t}"
			)

			ax.text(
				txpos, typos,
				label,
				fontsize=6,
				color="red",
				ha="center",
				va="center",
				zorder=5,
				bbox=dict(boxstyle="round,pad=0.15", fc="white", alpha=0.9, linewidth=0.25)
			)
	# bounds & aspect
	if xs and ys:
		xmin, xmax = min(xs), max(xs)
		ymin, ymax = min(ys), max(ys)
		dx = max(1e-6, xmax - xmin)
		dy = max(1e-6, ymax - ymin)
		margin = 0.06
		ax.set_xlim(xmin - dx * margin, xmax + dx * margin)
		ax.set_ylim(ymin - dy * margin, ymax + dy * margin)
	ax.set_aspect("equal", adjustable="box")
	ax.set_xlabel("x")
	ax.set_ylabel("y")
	ax.set_title("DCEL (coordinate-based IDs) : vertices & half-edges")


# ---------------- CLI / main ----------------
def main() -> int:
	p = argparse.ArgumentParser(description="Plot DCEL and annotate relations by coordinate-based IDs")
	p.add_argument("file", help="Path to GeoJSON file")
	p.add_argument("--tol", type=float, default=1e-9, help="Tolerance for coordinate equality (default 1e-9)")
	p.add_argument("--no-annot-verts", action="store_true", help="Do not annotate vertices")
	p.add_argument("--no-annot-edges", action="store_true", help="Do not annotate half-edges")
	p.add_argument("--no-vertex-relations", action="store_true", help="Do not show outgoing he relations on vertices")
	p.add_argument("--annot-relations", action="store_true", help="Also show verbose next/prev/twin/face for each half-edge (may clutter)")
	p.add_argument("--curv", type=float, default=0.2, help="Curvature fraction for arrows (visual only)")
	p.add_argument("-o", "--output", help="Save figure to path instead of showing it")
	args = p.parse_args()

	eps = args.tol

	# build config and parse
	cfg = dcel.Configuration()
	try:
		cfg.max_vertices = 10000
		cfg.max_half_edges = 10000
		cfg.max_faces = 10000
		cfg.position_tolerance = eps
	except Exception:
		pass

	builder = dcel.DCEL_Builder(cfg)
	ret = builder.Parse_GeoJSON_File(args.file)
	if (ret != pygeoflow.io.Error.NO_ERROR):
		print(f"Parsing failed, error code: {ret}", file=sys.stderr)
		return 5

	storage = builder.DCEL()

	fig, ax = plt.subplots(figsize=(12, 8))
	plot_dcel_with_coord_ids(storage=storage, ax=ax, eps=eps,
							 annotate_vertices=not args.no_annot_verts,
							 annotate_halfedges=not args.no_annot_edges,
							 annotate_vertex_relations=not args.no_vertex_relations,
							 annotate_relations=args.annot_relations,
							 curvature_frac=args.curv)

	if args.output:
		fig.savefig(args.output, dpi=300, bbox_inches="tight")
		print(f"Saved figure to {args.output}")
	else:
		plt.show()

	return 0


if __name__ == "__main__":
	raise SystemExit(main())