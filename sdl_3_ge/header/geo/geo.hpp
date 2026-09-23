#pragma once 
#include <vector>
#include <SDL3/SDL.h> 
#include <numeric> 

#include "../math/Vector.hpp" 
#include "common.hpp" 
#define PIE 3.1415
namespace SKC::Geo {
	namespace priv {
		bool is_ear(int idx, const vect_list &points) {
			auto pi = Math::calc_array_idx(idx, -1, points.size());
			auto ni = Math::calc_array_idx(idx, 1, points.size());
			auto prev = points[pi];
			auto next = points[ni];
			auto curr = points[idx];
			//NOTE(SKC): we're assuming that the polygon is in clockwise winding order for now.
			//TODO(SKC): we may want to make winding order a paramiter. 
			if (angle_of_corner(prev, curr, next) >= 0)  return false;
			for (auto point : points) {
				if (point == prev || point == next || point == curr) continue; 
				if (point_inside_triangle(point, prev, curr, next)) return false;
			}
			return true; 
		}
	}
	std::vector<int> ear_clip(vect_list points) {
		
		//NOTE(skc): I know that this is only useful data when the size is three
		//however the there's an implicit contract that size >= 3 so I am going to assume that 
		//if this is true size == 3 
		if (points.size() <= 3) return { 0, 1,2 }; 
		auto ret = std::vector<int>();
		auto indices  = std::vector<int>(points.size());
		std::iota(indices.begin(), indices.end(), 0); 
		int idx = 0; 
		while (indices.size() > 3) {
			
			if (priv::is_ear(idx, points)) {
				int n_curr = indices.size();
				auto prev = indices[Math::calc_array_idx(idx, -1, n_curr)];
				auto next = indices[Math::calc_array_idx(idx, 1, n_curr)];
				auto curr = indices[idx];
				ret.push_back(prev);
				ret.push_back(curr);
				ret.push_back(next);
				indices.erase(indices.begin() + idx);
				idx = 0;
				continue; 

			}
			if (idx >= indices.size()) {
				return {};
			}
			++idx; 
			
		}
		ret.append_range(indices);
		std::print("{}", ret);

		return ret; 
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