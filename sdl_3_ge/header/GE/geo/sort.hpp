#pragma once 
#include <algorithm>
#include <cstdint>

#include "common.hpp" 
#include "../../math/Vector.hpp"
namespace SKC::GE::Geo {
	namespace priv {
		constexpr size_t find_point_on_bounding_box(vect_list points) noexcept {
			size_t min_idx = 0; 
			for (size_t i = 1; i < points.size(); i++) {
				auto min = points[min_idx], point = points[i];
				bool y_less = point.y < min.y;
				bool y_same_x_less = (point.y == min.y && point.x < min.x);
				if (y_less || y_same_x_less) {
					min_idx = i;
				}
			};
			return min_idx; 
		}
	}
	void sort(vect_list &points )  {
		if (points.size() < 3) return; 
		std::swap(points[0], points[priv::find_point_on_bounding_box(points)]);
		std::sort(points.begin() + 1, points.end(),
			[anchor = points[0]](auto p1, auto p2) -> bool {
				auto o = angle_of_corner(anchor, p1, p2);
				if (o == 0) {
					return (anchor - p2).len2() > (anchor - p1).len2(); 
				}
				return o < 0;
			}
		); 
	}
}