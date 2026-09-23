#pragma once
#include <vector>
#include <SDL3/SDL_render.h> 
#include "../geo/common.hpp"
#include "./Color.hpp"
#include "../math/Math.hpp"
namespace SKC::GE {
	using vertex_list_t = std::vector<SDL_Vertex>;

	[[nodiscard]] vertex_list_t create_vertex_list(const Geo::vect_list &points, color vertcolor) {
		vertex_list_t verts = {};
		float r = (float)vertcolor.r / 255, g = (float)vertcolor.g / 255, b = (float)vertcolor.b / 255;	

		for (const auto& point : points) {
			verts.push_back(
				//TODO(SKC): make this accept alpha values!
				SDL_Vertex{
					.position = (SDL_FPoint)point,
					.color = { r,g,b,1 },
				}
				);
		}
		return verts;
	}
}