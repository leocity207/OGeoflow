"""
MILP-path style polyline simplification with octolinear angle constraints.

Main steps implemented:
 - insert_midpoints(polyline, times)
 - build_distance_matrix(polyline) -> DM (N,N,2), PM (N,N,2) storing breakpoints
 - compute area difference between sub-curve and candidate triangle-path (uses shapely if available)
 - search_explore(...) : depth-first search with lookahead F to choose R points minimizing alpha*angle_cost + beta*curve_distance
 - reconstruct resulting polyline.

Usage example at bottom.
"""

from typing import List, Tuple, Dict
import math
import numpy as np
from shapely.geometry import Polygon, LinearRing

Point = Tuple[float, float]
Polyline = List[Point]

# -------------------------
# Utilities
# -------------------------
def insert_midpoints(poly: Polyline, times: int = 1) -> Polyline:
	"""
	Insert midpoint between every consecutive pair `times` times (iteratively).
	If times = 1, each segment is split once; repeating increases point count exponentially.
	"""
	if times <= 0:
		return poly[:]
	res = poly[:]
	for _ in range(times):
		new = []
		for i in range(len(res)-1):
			new.append(res[i])
			x1,y1 = res[i]
			x2,y2 = res[i+1]
			mid = ((x1+x2)/2.0, (y1+y2)/2.0)
			new.append(mid)
		new.append(res[-1])
		res = new
	return res

def shoelace_area(coords: List[Point]) -> float:
	"""Signed shoelace area (positive or negative)."""
	n = len(coords)
	if n < 3:
		return 0.0
	s = 0.0
	for i in range(n):
		x1,y1 = coords[i]
		x2,y2 = coords[(i+1)%n]
		s += x1*y2 - x2*y1
	return 0.5 * s

def polygon_area_absolute(coords: List[Point]) -> float:
	"""
	Returns the absolute geometric area of the polygon defined by coords (closed ring).
	If shapely is available, uses it to compute correct area even for self-crossing polygons.
	Otherwise returns abs(shoelace) as fallback (may undercount for self-crossings).
	"""
	if len(coords) < 3:
		return 0.0

	ring = LinearRing(coords)
	poly = Polygon(ring)
	return abs(poly.area)


def area_between_polyline_and_triangle(subcurve: Polyline, tri: List[Point]) -> float:
	"""
	Compute absolute area between `subcurve` (list of points from P_i..P_j) and the 2-segment path tri = [P_i, P_k, P_j].
	Method: build closed polygon by following subcurve then returning along the triangle edges reversed,
	compute polygon area absolute (with shapely if available).
	"""
	if len(subcurve) < 2:
		return 0.0
	P_i = subcurve[0]
	P_j = subcurve[-1]
	P_k = tri[1]  # tri = [P_i, P_k, P_j]
	# Build polygon ring: subcurve forward, then (P_k, P_j, P_i) back to start forming closed loop
	ring = list(subcurve)
	# append the return path along triangle edges
	ring.append(P_k)
	ring.append(P_j)
	# close by repeating first automatically in area function
	return polygon_area_absolute(ring)

def abs_inf_norm_distance(a: Point, b: Point) -> float:
	"""L_infty distance between points."""
	return max(abs(a[0]-b[0]), abs(a[1]-b[1]))

def quantize_octolinear_direction(vec: Point) -> Tuple[int, Point]:
	"""
	Map a vector to one of 8 octolinear directions.
	Returns (dir_index in 0..7, unit_vector).
	Directions are multiples of 45 deg starting from (1,0).
	"""
	vx,vy = vec
	if vx == 0 and vy == 0:
		return (-1, (0.0,0.0))
	angle = math.atan2(vy, vx)  # -pi..pi
	# Map to 0..2pi
	if angle < 0:
		angle += 2*math.pi
	# Quantize to nearest multiple of 45 degrees
	sector = int(round(angle / (math.pi/4))) % 8
	# direction unit vectors
	theta = sector * (math.pi/4)
	ux,uy = math.cos(theta), math.sin(theta)
	return sector, (ux,uy)

def angle_cost_between(prev_dir_idx: int, cur_vec: Point, angle_cost_value: float = 1.0) -> Tuple[float,int]:
	"""
	Compute angle cost between previous octolinear direction index and current vector.
	If prev_dir_idx == -1 (no previous), cost = 0.
	Returns (cost, cur_dir_idx)
	The cost scheme: 0 if same octolinear direction, angle_cost_value otherwise.
	"""
	cur_idx, _ = quantize_octolinear_direction(cur_vec)
	if prev_dir_idx == -1 or cur_idx == -1:
		return 0.0, cur_idx
	if cur_idx == prev_dir_idx:
		return 0.0, cur_idx
	else:
		return angle_cost_value, cur_idx

# -------------------------
# Build Distance Matrix DM and PM
# -------------------------
def build_distance_matrix(poly: Polyline) -> Tuple[np.ndarray, Dict[Tuple[int,int,int], Point]]:
	"""
	Build DM (N,N,2) and PM mapping as described.

	For each i<j:
	  compute dx = abs(x_i-x_j), dy = abs(y_i-y_j)
	  if dx > dy:
		P_k1 = (x_j - dy_sign, y_i) ; P_k2 = (x_i + dy_sign, y_j)
		where dy_sign = abs(y_i-y_j) with sign decided to keep integer? We'll use numeric exact value but
		keep sign so points are aligned as described.
	  else:
		P_k1 = (x_i, y_j - dx) ; P_k2 = (x_j, y_i + dx)

	Then compute DM[i,j,0] = area between subcurve P_i..P_j and triangle (P_i, P_k1, P_j),
				   DM[i,j,1] = area between subcurve P_i..P_j and triangle (P_i, P_k2, P_j)
	Also fill PM[(i,j,k)] = the corresponding P_k (k=0 or 1)
	For i >= j, fill with zeros or inf (not used).
	"""
	N = len(poly)
	DM = np.full((N, N, 2), np.inf, dtype=float)
	PM = {}  # key (i,j,k) -> Point
	for i in range(N):
		for j in range(i+1, N):
			xi, yi = poly[i]
			xj, yj = poly[j]
			dx = abs(xi - xj)
			dy = abs(yi - yj)

			if dx > dy:
				# sign for y difference: we must preserve direction to place candidate points on grid aligned
				sign_y = math.copysign(1.0, yj - yi) if yj != yi else 1.0
				# compute absolute shift magnitude = dy
				P_k1 = (xj - dy * sign_y, yi)
				P_k2 = (xi + dy * sign_y, yj)
			else:
				sign_x = math.copysign(1.0, xj - xi) if xj != xi else 1.0
				P_k1 = (xi, yj - dx * sign_x)
				P_k2 = (xj, yi + dx * sign_x)

			# subcurve points
			subcurve = poly[i:j+1]
			# triangles
			tri1 = [poly[i], P_k1, poly[j]]
			tri2 = [poly[i], P_k2, poly[j]]
			# compute absolute areas between subcurve and each triangle path
			a1 = area_between_polyline_and_triangle(subcurve, tri1)
			a2 = area_between_polyline_and_triangle(subcurve, tri2)

			DM[i,j,0] = a1
			DM[i,j,1] = a2
			PM[(i,j,0)] = P_k1
			PM[(i,j,1)] = P_k2
	return DM, PM

# -------------------------
# Exploratory search / DP
# -------------------------
def search_polyline(poly: Polyline,
					DM: np.ndarray,
					PM: Dict[Tuple[int,int,int], Point],
					R: int,
					F: int,
					alpha: float = 1.0,
					beta: float = 1.0,
					angle_cost_value: float = 1.0,
					max_solutions: int = 1) -> List[Dict]:
	"""
	Explore choices of R points (including first and last fixed) from poly (length N) with bounding lookahead F.
	Returns up to max_solutions best solutions (sorted by cost). Each solution dict contains:
	  - 'indices': chosen indices in poly (length R)
	  - 'k_choices': list of k in {0,1} for each segment between chosen points (length R-1)
	  - 'polyline': rebuilt polyline as list of Points (using PM choices)
	  - 'cost': combined cost
	  - 'angle_cost', 'curve_cost'
	Method: DFS with pruning: from current index idx_pos (index in original poly),
	  examine next candidate indices among next F available points; when fewer than needed remain, finish exact.
	Note: This is an exponential search; pruning and F choices control complexity.
	"""

	N = len(poly)
	assert R >= 2 and R <= N, "R must be between 2 and N"
	start_idx = 0
	end_idx = N-1
	required_to_pick = R - 2  # excluding first and last
	best_solutions = []

	# caching computed costs between any two indices for both k choices already in DM
	# DFS stack: (chosen_indices_list, prev_dir_idx, current_costs (angle_sum, curve_sum))
	# Initially chosen has start_idx
	def dfs(chosen_indices, prev_dir_idx, angle_sum, curve_sum):
		# If we've selected enough intermediate points, forcibly add end and compute final costs
		if len(chosen_indices) == R-1:
			# need to append end
			last = chosen_indices[-1]
			i = last
			j = end_idx
			# for this last segment choose best k (0 or 1) with angle cost considered
			best_seg_cost = None
			best_k = 0
			best_new_dir = prev_dir_idx
			for k in (0,1):
				# vector for direction approximate: from PM[(i,j,k)]? We want vector P_i -> P_k or to P_j? For octolinear angle we consider the directed segment used:
				# We will approximate current vector as (P_i -> P_j) for direction quantization, since final path is P_i -> P_k -> P_j but we treat direction at vertex by P_i->P_k or P_k->P_j depending.
				# Simpler heuristic: direction of chord P_i->P_j
				vec = (poly[j][0]-poly[i][0], poly[j][1]-poly[i][1])
				a_cost, cur_idx = angle_cost_between(prev_dir_idx, vec, angle_cost_value)
				curve = float(DM[i,j,k])
				seg_cost = alpha * a_cost + beta * curve
				if best_seg_cost is None or seg_cost < best_seg_cost:
					best_seg_cost = seg_cost
					best_k = k
					best_new_dir = cur_idx
			total_angle = angle_sum + (best_seg_cost - beta * DM[i,j,best_k]) / alpha if alpha != 0 else angle_sum
			total_curve = curve_sum + DM[i,j,best_k]
			total_cost = alpha * (total_angle) + beta * (total_curve)
			# build full solution indices list
			full_indices = chosen_indices + [end_idx]
			# Reconstruct polyline using PM choices for each segment between indices
			k_choices = []
			out_poly = []
			for t in range(len(full_indices)-1):
				ii = full_indices[t]
				jj = full_indices[t+1]
				# choose better k for each segment under the prev_dir logic:
				# For reconstruction we re-evaluate best k minimizing alpha*angle + beta*curve locally,
				bestk = 0
				bestseg = None
				for k in (0,1):
					vec = (poly[jj][0]-poly[ii][0], poly[jj][1]-poly[ii][1])
					# but angle at interior vertex requires previous segment's direction. For simplicity, pick k with min curve if tie.
					a_cost_val = 0.0
					segcost = alpha * a_cost_val + beta * DM[ii,jj,k]
					if bestseg is None or segcost < bestseg:
						bestseg = segcost
						bestk = k
				k_choices.append(bestk)
				# add P_i then its chosen mid-vertex to out_poly (we want poly made of P_i,P_k,P_j chained)
				if t == 0:
					out_poly.append(poly[ii])
				pk = PM.get((ii,jj,k_choices[-1]), None)
				if pk is not None:
					out_poly.append(pk)
				out_poly.append(poly[jj])
			sol = {
				'indices': full_indices,
				'k_choices': k_choices,
				'polyline': out_poly,
				'cost': total_cost,
				'angle_cost': total_angle,
				'curve_cost': total_curve
			}
			# keep best solutions list limited to max_solutions
			best_solutions.append(sol)
			# sort and trim
			best_solutions.sort(key=lambda s: s['cost'])
			if len(best_solutions) > max_solutions:
				best_solutions[:] = best_solutions[:max_solutions]
			return

		# Otherwise choose next intermediate point among next F points after current last chosen index
		current_idx = chosen_indices[-1]
		# compute how many must still be chosen
		chosen_so_far = len(chosen_indices) - 1  # excluding start
		remaining_needed = required_to_pick - chosen_so_far
		# next candidate indices: from current_idx+1 up to up-to N-2 (can't pick last) but ensure enough points left
		max_next = min(N-2, current_idx + F)  # N-2 because last index is fixed
		for candidate in range(current_idx+1, max_next+1):
			# check feasibility: after picking 'candidate' we must still be able to pick remaining_needed-1 points from available
			available_after = (N-1) - candidate - 1  # excluding last element; number of free spots left
			if available_after < (remaining_needed - 1):
				continue  # not enough points left to fill required R
			# compute costs to go from current_idx to candidate
			i = current_idx
			j = candidate
			# for each k in {0,1} we evaluate local best (angle + curve)
			for k in (0,1):
				curve = float(DM[i,j,k])
				# derive direction vector approx as chord i->j
				vec = (poly[j][0]-poly[i][0], poly[j][1]-poly[i][1])
				a_cost_val, new_dir_idx = angle_cost_between(prev_dir_idx, vec, angle_cost_value)
				seg_cost = alpha * a_cost_val + beta * curve
				# proceed deeper
				dfs(chosen_indices + [candidate], new_dir_idx, angle_sum + a_cost_val, curve_sum + curve)
		# end for

	# start dfs from start index
	dfs([start_idx], -1, 0.0, 0.0)

	return best_solutions

# -------------------------
# Example wrapper to run whole pipeline
# -------------------------
def simplify_polyline(poly: Polyline,
					  times_midpoints: int,
					  R: int,
					  F: int,
					  alpha: float = 1.0,
					  beta: float = 1.0,
					  angle_cost_value: float = 1.0,
					  max_solutions: int = 1) -> List[Dict]:
	"""
	Full pipeline:
	 1) insert midpoints `times_midpoints` times
	 2) build DM and PM
	 3) run search to find R-point simplification with lookahead F
	Returns list of up to max_solutions dictionaries (see search output).
	"""
	expanded = insert_midpoints(poly, times_midpoints)
	DM, PM = build_distance_matrix(expanded)
	sols = search_polyline(expanded, DM, PM, R=R, F=F, alpha=alpha, beta=beta,
						   angle_cost_value=angle_cost_value, max_solutions=max_solutions)
	return sols

# -------------------------
# Example usage
# -------------------------
if __name__ == "__main__":
	# Simple toy polyline (a crooked polyline)
	poly_in = [
		(0.0, 0.0),
		(5.0, 2),
		(10.0, 1),
		(15.0, 10),
		(17.0, 11),
		(20.0, 5)
	]
	# parameters
	times_midpoints = 1  # increase N
	R = 4                # target points in output polyline
	F = 2                # branching / lookahead
	alpha = 1.0
	beta = 1.0

	sols = simplify_polyline(poly_in, times_midpoints, R, F, alpha, beta, angle_cost_value=1.0, max_solutions=2)
	for idx, s in enumerate(sols):
		print(f"Solution #{idx+1}: cost={s['cost']:.4f}, angle_cost={s['angle_cost']:.4f}, curve_cost={s['curve_cost']:.4f}")
		print("Indices:", s['indices'])
		print("k choices:", s['k_choices'])
		print("Result polyline points:")
		for p in s['polyline']:
			print("  ", p)
		print()
