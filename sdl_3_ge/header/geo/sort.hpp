#pragma once 
#include <algorithm>
#include <cstdint>

#include "common.hpp" 
#include "../math/Vector.hpp"
namespace SKC::Geo {

// NOTE(SKC): 
// sorts the points based off of their angle from the center of mass. 
// this is not guaranteed to work if the shape has a concave vertex that is 
// past the center point. 
// in order to properly render those you would have to manually sort them! 

	void sort(vect_list& points) {
		if (points.size() < 3) {
			return; 
		}
		std::sort(points.begin(), points.end(),
			[anchor = calculate_mean_point(points)](auto p1, auto p2) mutable -> bool {
				auto o = angle_of_corner( p1, anchor, p2);
				if (o == 0) {
					auto length1 = (anchor - p2).len2();
					auto length2 = (anchor - p2).len2(); 
					return (length1 - length2) > 0;
				}
				return o > 0;
			}
		);
	}
}