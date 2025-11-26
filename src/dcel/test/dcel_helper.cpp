#include "include/dcel/builder.h"
#include <gtest/gtest.h>

#include <cmath>
#include <optional>
#include <vector>
#include <string>

bool Nearly_Equal(double a, double b, double eps = 1e-9) 
{
	return std::fabs(a - b) <= eps;
}

// Compare Positions
bool Positions_Equal(const O::GeoJSON::Position& a, const O::GeoJSON::Position& b)
{
	if (!Nearly_Equal(a.longitude, b.longitude)) return false;
	if (!Nearly_Equal(a.latitude, b.latitude)) return false;
	if (a.altitude.has_value() != b.altitude.has_value()) return false;
	if (a.altitude.has_value() && b.altitude.has_value()) {
		if (!Nearly_Equal(*a.altitude, *b.altitude)) return false;
	}
	return true;
}

// Compare rings: vector<Position> with optional closing repeated vertex
bool Ring_Equal(const std::vector<O::GeoJSON::Position>& r1, const std::vector<O::GeoJSON::Position>& r2)
{
	if (r1.size() != r2.size()) return false;
	for (size_t i = 0; i < r1.size(); ++i) {
		if (!Positions_Equal(r1[i], r2[i])) return false;
	}
	return true;
}

// Compare Polygons (rings)
bool Polygon_Equal(const O::GeoJSON::Polygon& p1, const O::GeoJSON::Polygon& p2)
{
	if (p1.rings.size() != p2.rings.size()) return false;
	for (size_t i = 0; i < p1.rings.size(); ++i)
		if (!Ring_Equal(p1.rings[i], p2.rings[i])) return false;
	return true;
}

// Compare MultiPolygon
bool Multi_Polygon_Equal(const O::GeoJSON::Multi_Polygon& m1, const O::GeoJSON::Multi_Polygon& m2)
{
	if (m1.polygons.size() != m2.polygons.size()) return false;
	for (size_t i = 0; i < m1.polygons.size(); ++i)
		if (!Polygon_Equal(m1.polygons[i], m2.polygons[i])) return false;
	return true;
}

// Compare Property (simple deep-equality for common scalar cases used in tests)
bool Property_Equal(const O::GeoJSON::Property& a, const O::GeoJSON::Property& b)
{
	if (a.Is_Null() && b.Is_Null()) return true;
	if (a.Is_Bool() && b.Is_Bool()) return a.Get_Bool() == b.Get_Bool();
	if (a.Is_Integer() && b.Is_Integer()) return a.Get_Int() == b.Get_Int();
	if (a.Is_Double() && b.Is_Double()) return Nearly_Equal(a.Get_Double(), b.Get_Double());
	if (a.Is_String() && b.Is_String()) return std::string(a.Get_String()) == std::string(b.Get_String());
	if (a.Is_Array() && b.Is_Array()) {
		const auto& A = a.Get_Array();
		const auto& B = b.Get_Array();
		if (A.size() != B.size()) return false;
		for (size_t i = 0; i < A.size(); ++i) if (!Property_Equal(A[i], B[i])) return false;
		return true;
	}
	if (a.Is_Object() && b.Is_Object()) {
		const auto& A = a.Get_Object();
		const auto& B = b.Get_Object();
		if (A.size() != B.size()) return false;
		for (auto const& [k, va] : A) {
			auto it = B.find(k);
			if (it == B.end()) return false;
			if (!Property_Equal(va, it->second)) return false;
		}
		return true;
	}
	return false;
}

// Compare Feature's properties, id and bbox and geometry
void Assert_Features_Equal(const O::GeoJSON::Feature& expected, const O::GeoJSON::Feature& actual)
{
	ASSERT_TRUE(expected.geometry.has_value() && actual.geometry.has_value());

	const auto& ge = expected.geometry.value();
	const auto& ga = actual.geometry.value();

	// Compare id
	ASSERT_EQ(expected.id.has_value(), actual.id.has_value());
	if (expected.id.has_value()) ASSERT_EQ(*expected.id, *actual.id);

	// Compare bbox
	ASSERT_EQ(expected.bbox.has_value(), actual.bbox.has_value());
	if (expected.bbox.has_value()) {
		const auto& be = expected.bbox->Get();
		const auto& ba = actual.bbox->Get();
		ASSERT_EQ(be.size(), ba.size());
		for (size_t i = 0; i < be.size(); ++i) ASSERT_TRUE(Nearly_Equal(be[i], ba[i]));
	}

	// Compare properties (basic)
	ASSERT_TRUE(Property_Equal(expected.properties, actual.properties));

	// Compare geometry type and coordinates
	if (ge.Is_Polygon()) {
		ASSERT_TRUE(ga.Is_Polygon());
		ASSERT_TRUE(Polygon_Equal(ge.Get_Polygon(), ga.Get_Polygon()));
	} else if (ge.Is_Multi_Polygon()) {
		ASSERT_TRUE(ga.Is_Multi_Polygon());
		ASSERT_TRUE(Multi_Polygon_Equal(ge.Get_Multi_Polygon(), ga.Get_Multi_Polygon()));
	} else {
		FAIL() << "Test did not create expected geometry type";
	}
}

O::GeoJSON::Feature Make_Simple_Polygon_Feature(const std::string& id, const std::vector<std::vector<std::pair<double,double>>>& ringsCoords, const O::GeoJSON::Property& properties, std::optional<O::GeoJSON::Bbox> bbox = std::nullopt)
{
	O::GeoJSON::Feature f;
	O::GeoJSON::Geometry g;
	O::GeoJSON::Polygon poly;

	for (const auto& ring : ringsCoords) {
		std::vector<O::GeoJSON::Position> r;
		for (const auto& p : ring) {
			O::GeoJSON::Position pos;
			pos.longitude = p.first;
			pos.latitude = p.second;
			r.push_back(pos);
		}
		// close ring if not closed
		if (!r.empty() && !(Nearly_Equal(r.front().longitude, r.back().longitude) && Nearly_Equal(r.front().latitude, r.back().latitude))) {
			r.push_back(r.front());
		}
		poly.rings.push_back(std::move(r));
	}

	g.value = poly;
	f.geometry = std::move(g);
	f.properties = properties;
	f.id = id;
	if (bbox.has_value()) f.bbox = bbox;
	return f;
}

O::GeoJSON::Feature Make_Multi_Polygon_Feature(const std::string& id, const std::vector<std::vector<std::vector<std::pair<double,double>>>>& polygonsRings, const O::GeoJSON::Property& properties, std::optional<O::GeoJSON::Bbox> bbox = std::nullopt)
{
	O::GeoJSON::Feature f;
	O::GeoJSON::Geometry g;
	O::GeoJSON::Multi_Polygon mp;

	for (const auto& polygonRings : polygonsRings) {
		O::GeoJSON::Polygon poly;
		for (const auto& ring : polygonRings) {
			std::vector<O::GeoJSON::Position> r;
			for (const auto& p : ring) {
				O::GeoJSON::Position pos; pos.longitude = p.first; pos.latitude = p.second;
				r.push_back(pos);
			}
			// close
			if (!r.empty() && !(Nearly_Equal(r.front().longitude, r.back().longitude) && Nearly_Equal(r.front().latitude, r.back().latitude))) {
				r.push_back(r.front());
			}
			poly.rings.push_back(std::move(r));
		}
		mp.polygons.push_back(std::move(poly));
	}

	g.value = mp;
	f.geometry = std::move(g);
	f.properties = properties;
	f.id = id;
	if (bbox.has_value()) f.bbox = bbox;
	return f;
}

