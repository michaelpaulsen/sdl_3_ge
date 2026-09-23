#pragma once 
#include <vector>
#include <SDL3/SDL.h>
#include "../math/Vector.hpp"
namespace SKC::Geo {
	using point_t = Math::Vect2f; 
	using vect_list = std::vector<point_t>;
	
	//SA https://en.wikipedia.org/wiki/Cross_product
	// returns ∠ABC (the angle formed by the corner ABC) 
	// when used for sorting this should be called like this 
	// angle_of_corner(P1, center, P2) 
	double angle_of_corner(point_t& A, point_t& B, point_t& C) {
		auto edge1 = (B - A);
		auto edge2 = (C - B);
		edge1.normalize();
		edge2.normalize();
		return edge1.cross(edge2);
	}
	bool point_inside_triangle(point_t p, point_t  a, point_t b, point_t c) {
		// In Y-down, a visual CW triangle
		// requires all results to
		// be strictly negitive
		return (angle_of_corner(a, p, b) < 0) &&
			   (angle_of_corner(b, p, c) < 0) &&
			   (angle_of_corner(c, p, a) < 0);
	};
	auto calculate_mean_point(const vect_list& points) {
		Geo::point_t center{ 0,0 };
		for (const auto& point : points) {
			center += point;
		}
		return center / points.size();
	}

}