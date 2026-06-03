#pragma once 
#include <vector>
#include <SDL3/SDL.h> 
#include "../math/Vector.hpp" 
#include "../math/Math.hpp"
#include "Color.hpp"
#include <ranges>
namespace SKC::GE {
	//todo(skc) : make an overload of this that accepts per vertex colors. 
	using vertex_list_t = std::vector<SDL_Vertex>; 
	vertex_list_t create_vertex_list(std::vector<Math::Vect2d> points, color vertcolor) {
		vertex_list_t verts = {}; 
		Math::Vect2d bounding_box_max = { -INFINITY, -INFINITY };
		Math::Vect2d bounding_box_min = { INFINITY, INFINITY };
		namespace rng = std::ranges; 
		//we need to calculate the bounding box for the verticies so that we can set the UVs and so 
		//that we can estimate the center of mass. 
		for (const auto p : points) {
			if (bounding_box_max.x <= p.x) bounding_box_max.x = p.x;
			if (bounding_box_max.y <= p.y) bounding_box_max.y = p.y;
			if (bounding_box_min.x >= p.x) bounding_box_min.x = p.x;
			if (bounding_box_min.y >= p.y) bounding_box_min.y = p.y;
		}
		auto center_of_mass = ((bounding_box_max - bounding_box_min) / 2) + bounding_box_min;
		float r = (float)vertcolor.r / 255, g = (float)vertcolor.g / 255, b = (float)vertcolor.b / 255; 
		float u = (float)SKC::Math::map(center_of_mass.x, bounding_box_min.x, bounding_box_max.x, 0, 1);
		float v = (float)SKC::Math::map(center_of_mass.y, bounding_box_min.y, bounding_box_max.y, 0, 1);
		SDL_Vertex vert = { .position = {(float)center_of_mass.x, (float)center_of_mass.y}, .color = {r,g,b}, .tex_coord{u, v} }; 
		//NOTE(skc) : I am sure that this copy won't be a perf bottle neck its only 32 bytes... ... 
		verts.push_back(vert);
		//we have to sort these because it is requred for triagulation.  
		rng::sort(points, [center_of_mass](auto p1, auto p2) {
			//TODO(skc): make a angle_from function in the Vect2 class! 
			//this is a common enough operaton that it should be in the Vect2 class!
			auto dist1 = p1 - center_of_mass;
			auto dist2 = p2 - center_of_mass;
			auto p1a = atan2(dist1.x, dist1.y);
			auto p2a = atan2(dist2.x, dist2.y);
			return p1a > p2a;
		}); 
		for (const auto& point : points) {
			u = (float)SKC::Math::map(point.x, bounding_box_min.x, bounding_box_max.x, 0, 1);
			v = (float)SKC::Math::map(point.y, bounding_box_min.y, bounding_box_max.y, 0, 1);
			
			verts.push_back(
				SDL_Vertex{
					.position = point,
					.color = {r,g,b},
					.tex_coord = {u,v}
				}
			);
		}

		return verts; 
		
	}
	std::vector<int> build_index_list(size_t count) {
		//assert(center_idx <= count && "center_idx must be a valid index");
		auto idx = std::vector<int>(); 
		for (int x = 0; x < count - 1 ; ++x) {
			idx.push_back(x);
			idx.push_back(0);
			idx.push_back(x+ 1);

		}
		idx.push_back(count-1);
		idx.push_back(0);
		idx.push_back(1);
		return idx; 
	}
}