from typing import Dict, List, Any
from polygon_registered import Polygon
from line_string import Line_String
from utils import Logger
from shapely.geometry import Polygon as ShapelyPolygon
from shapely.ops import unary_union


class Topology:

	
	#: string of the underlying geojson
	_json: Dict[str,Any]
	_polygons: List[Polygon]
	_Linestrings: List[Line_String]

	@staticmethod
	def Extract_All_Polygons(json: Dict[str,Any]) -> List[Polygon]:
		polygons = []
		polygon_id = 0
		for feature in json["features"]:
			geometry_type = feature.get("geometry", {}).get("type","")
			geometry_coordinate = feature.get("geometry").get("coordinates")
			if geometry_type == "Polygon":
				polygons.append(Polygon(feature, geometry_coordinate, polygon_id))
			elif geometry_type == "MultiPolygon" :
				for sub_coordinate in geometry_coordinate:
					polygons.append(Polygon(feature, sub_coordinate, polygon_id))
			else:
				Logger.Warning(f"the geometry with geometry type {geometry_type} cannot be parsed")
				continue
			polygon_id +=1

	@staticmethod
	def Create_Topology(polygons: List[Polygon]):
		linestrings = []
		for i,polygon in zip(range(len(polygons)),polygons):
			geom_i = ShapelyPolygon(polygon.Get_Coordinates(), polygon.Get_Extrusions())
			for j in range(i + 1, len(polygons)):
				geom_j = ShapelyPolygon(polygons[j].Get_Coordinates(), polygons[j].Get_Extrusions())

				inter = geom_i.boundary.intersection(geom_j.boundary)

				if inter.is_empty:
					continue

				# Normalize into a list of lines
				shared_segments = []
				if inter.geom_type == "LineString":
					shared_segments = [inter]

				elif inter.geom_type == "MultiLineString":
					shared_segments = list(inter.geoms)

				for seg in shared_segments:
					if seg.length == 0:
						continue
					linestring = Line_String(seg.coords)
					linestrings.append(linestring)
					polygon.Register_Linestring(linestring)
					polygons[j].Register_Linestring(linestring)

			shared_union = unary_union(polygon.Get_Lines())
			remainign = geom_i.boundary.difference(shared_union)
			if remainign.is_empty:
				continue
			if remaining.geom_type == "LineString":
				remaining = [remaining]
			else:
				remaining = list(remaining.geoms)
			
			for seg in remaining:
				if seg.length == 0:
					continue
				ls = Line_String(seg.coords)
				linestrings.append(ls)
				polygon.Register_Linestring(ls)



	def __init__(self, json: Dict[str,Any]):
		self._json = json
		self._polygons = Topology.Extract_All_Polygons(json)
		self._Linestrings = Topology.Create_Topology(self._polygons)

	def Get_Lines(self):
		return self._Linestrings

