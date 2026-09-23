#pragma once 
#include <vector>
#include <SDL3/SDL.h>
#include "../math/Vector.hpp"
namespace SKC::Geo {
	using point_t = Math::Vect2f; 
	using vect_list = std::vector<point_t>;
	
	// returns (B-A) X (C-B) 
	//SA https://en.wikipedia.org/wiki/Cross_product
	double angle_of_corner(point_t A, point_t B, point_t C) {
		return (B - A).cross(C - B);
	}
	bool point_inside_triangle(point_t p, point_t  a, point_t b, point_t c) {
		//const double EPSILON = 1e-9;
		double cross1 = angle_of_corner( a, p, b);
		double cross2 = angle_of_corner( b, p, c);
		double cross3 = angle_of_corner( c, p, a);
		// In Y-down, a visual CW triangle requires all results to be strictly positive
		auto has_negitive = (cross1 < 0) || (cross2 < 0) || (cross3 < 0);
		auto has_positive = (cross1 > 0) || (cross2 > 0) || (cross3 > 0);


		return !(has_negitive && has_positive);
	};

}