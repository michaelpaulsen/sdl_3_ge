#pragma once 
#include <vector>
#include <SDL3/SDL.h>
#include "../../math/Vector.hpp"
namespace SKC::GE::Geo {
	using vertex_list_t = std::vector<SDL_Vertex>;
	using vect_list = std::vector<Math::Vect2d>;
	
	// returns (B-A) X (C-B) 
	//SA https://en.wikipedia.org/wiki/Cross_product
	double angle_of_corner(skc_Vect2d A, skc_Vect2d B, skc_Vect2d C) {
		return (B - A).cross(C - B);
	}

	bool point_inside_triangle(skc_Vect2d p, skc_Vect2d  a, skc_Vect2d b, skc_Vect2d c) {
		const double EPSILON = 1e-9;
		double cross1 = (b - a).cross(p - a);
		double cross2 = (c - b).cross(p - b);
		double cross3 = (a - c).cross(p - c);
		// In Y-down, a visual CW triangle requires all results to be strictly positive
		return (cross1 > EPSILON) && (cross2 > EPSILON) && (cross3 > EPSILON);
	};

}