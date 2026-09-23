#pragma once
#include <vector>
#include <SDL3/SDL_render.h> 
#include "../geo/common.hpp"
#include "./Color.hpp"
#include "../math/Math.hpp"
namespace SKC::GE {
	using vertex_list_t = std::vector<SDL_Vertex>;

	[[nodiscard]] vertex_list_t create_vertex_list(Geo::vect_list points, color vertcolor) {
		vertex_list_t verts = {};
		float r = (float)vertcolor.r / 255, g = (float)vertcolor.g / 255, b = (float)vertcolor.b / 255;
		float minx = points[0].x, maxx = points[0].x,
			  miny = points[0].y, maxy = points[0].y;

		for (const auto& point : points) {
			if (point.x > maxx) maxx = point.x; 
			if (point.x < minx) minx = point.x; 
			if (point.y > maxy) maxy = point.y; 
			if (point.y < miny) miny = point.y; 
		}

		for (const auto& point : points) {
			float u = (float)SKC::Math::map(point.x, minx, maxx, 0, 1),
				  v = (float)SKC::Math::map(point.y, miny, maxy, 0, 1);
			verts.push_back(
				SDL_Vertex{
					.position = (SDL_FPoint)point,
					.color = { r,g,b,1 },
					.tex_coord ={u,v}
				}
				);
		}
		return verts;
	}
}