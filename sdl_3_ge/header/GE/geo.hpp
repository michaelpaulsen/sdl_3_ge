#pragma once 
#include <vector>
#include <SDL3/SDL.h> 
#include "../math/Vector.hpp" 
#include "../math/Math.hpp"
#include "../math/numbers.hpp"
#include "Color.hpp"
#include <ranges>
#define PIE 3.1415
namespace SKC::GE {
	using vertex_list_t = std::vector<SDL_Vertex>; 
	using vect_list = std::vector<Math::Vect2d>; 
	[[nodiscard]] vertex_list_t create_vertex_list(vect_list points, color vertcolor) {
		vertex_list_t verts = {}; 
		namespace rng = std::ranges; 
		//we need to calculate the bounding box for the verticies so that we can set the UVs and so 
		//that we can estimate the center of mass. 
		float r = (float)vertcolor.r / 255, g = (float)vertcolor.g / 255, b = (float)vertcolor.b / 255;
		for (const auto& point : points) {
			verts.push_back(
				SDL_Vertex{
					.position = (SDL_FPoint)point,
					.color = {r,g,b,1},
				}
			);
		}
		return verts;
	}
	std::vector<int> create_index_list(size_t count) {
		auto idx = std::vector<int>(); 
		for (int x = 1; x < count - 1 ; ++x) {
			idx.push_back(x);
			idx.push_back(0);
			idx.push_back(x+ 1);

		}
		return idx; 
	}
}