#!/usr/bin/env python3
from __future__ import annotations

import argparse
import sys
import matplotlib.pyplot as plt

import pygeoflow
from pygeoflow import io as gio


# ---------------------------------------------------------------------
# Conversion helpers (same idea as before, minimal)
# ---------------------------------------------------------------------

def position_to_xy(pos):
	if pos.altitude is None:
		return pos.longitude, pos.latitude
	return pos.longitude, pos.latitude


def plot_geometry(ax, geom):
	if geom["type"] == "Point":
		x, y = geom["coordinates"]
		ax.plot(x, y, "o")

	elif geom["type"] == "MultiPoint":
		xs, ys = zip(*geom["coordinates"])
		ax.plot(xs, ys, "o")

	elif geom["type"] == "LineString":
		xs, ys = zip(*geom["coordinates"])
		ax.plot(xs, ys, "-")

	elif geom["type"] == "MultiLineString":
		for line in geom["coordinates"]:
			xs, ys = zip(*line)
			ax.plot(xs, ys, "-")

	elif geom["type"] == "Polygon":
		for ring in geom["coordinates"]:
			xs, ys = zip(*ring)
			ax.plot(xs, ys, "-")

	elif geom["type"] == "MultiPolygon":
		for poly in geom["coordinates"]:
			for ring in poly:
				xs, ys = zip(*ring)
				ax.plot(xs, ys, "-")

	elif geom["type"] == "GeometryCollection":
		for g in geom["geometries"]:
			plot_geometry(ax, g)


def geometry_to_py(g):
	if g.Is_Point():
		p = g.Get_Point()
		return {"type": "Point", "coordinates": position_to_xy(p.position)}

	if g.Is_Multi_Point():
		mp = g.Get_Multi_Point()
		return {"type": "MultiPoint", "coordinates": [position_to_xy(p.position) for p in mp.points]}

	if g.Is_Line_String():
		ls = g.Get_Line_String()
		return {"type": "LineString", "coordinates": [position_to_xy(p) for p in ls.positions]}

	if g.Is_Multi_Line_String():
		mls = g.Get_Multi_Line_String()
		return {
			"type": "MultiLineString",
			"coordinates": [[position_to_xy(p) for p in ls.positions] for ls in mls.line_strings],
		}

	if g.Is_Polygon():
		poly = g.Get_Polygon()
		return {
			"type": "Polygon",
			"coordinates": [[position_to_xy(p) for p in ring] for ring in poly.rings],
		}

	if g.Is_Multi_Polygon():
		mp = g.Get_Multi_Polygon()
		return {
			"type": "MultiPolygon",
			"coordinates": [
				[[position_to_xy(p) for p in ring] for ring in poly.rings]
				for poly in mp.polygons
			],
		}

	if g.Is_Geometry_Collection():
		gc = g.Get_Geometry_Collection()
		return {"type": "GeometryCollection", "geometries": [geometry_to_py(g) for g in gc.geometries]}

	return {"type": "Unknown"}


def root_to_geometries(root):
	geoms = []

	if root.Is_Geometry():
		geoms.append(geometry_to_py(root.Get_Geometry()))

	elif root.Is_Feature():
		f = root.Get_Feature()
		if f.geometry:
			geoms.append(geometry_to_py(f.geometry))

	elif root.Is_Feature_Collection():
		fc = root.Get_Feature_Collection()
		for f in fc.features:
			if f.geometry:
				geoms.append(geometry_to_py(f.geometry))

	return geoms


# ---------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------

def main():
	parser = argparse.ArgumentParser(description="Plot a GeoJSON file")
	parser.add_argument("file", help="Path to GeoJSON file")
	args = parser.parse_args()

	p = gio.FullParser()
	err = p.Parse_From_File(args.file)
	if err != gio.Error.NO_ERROR:
		print(f"Parse error: {err}", file=sys.stderr)
		return 1

	root = p.Get_Geojson()
	geometries = root_to_geometries(root)

	if not geometries:
		print("No geometry found.", file=sys.stderr)
		return 2

	fig, ax = plt.subplots()
	for g in geometries:
		plot_geometry(ax, g)

	ax.set_aspect("equal", adjustable="box")
	ax.set_xlabel("Longitude")
	ax.set_ylabel("Latitude")
	ax.set_title("GeoJSON Plot")

	plt.show()


if __name__ == "__main__":
	main()