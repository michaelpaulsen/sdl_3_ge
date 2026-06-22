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

	

}