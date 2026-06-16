#pragma once 
#include "./Vector.hpp"
#include "./numbers.hpp"
#include <vector>
#include <functional>
namespace SKC::Math {
	Vect2<double> cast_ray(double theta, double r, SKC::Math::Vect2d start_pos = { 0,0 }) {
		Vect2d a = Vect2d::from_polar(theta, r); 
		return a + start_pos; 
	}
	std::vector<Vect2d> 
	cast_rays(
		double fov,
		double angle, 
		SKC::Math::Vect2d start_pos,
		double radius_step,
		double angle_steps,  
		std::function<bool (Vect2d, Vect2d, double)> pred 
	) {
		std::vector<Vect2d> points = {}; 
		for (double theta = -(fov / 2); theta <= fov / 2; theta += fov / angle_steps) {
			double r = 0; 
			auto ray = cast_ray((theta + angle) * SKC::Numbers::DEG_TO_RAD, r, start_pos); 
			while (pred(ray, start_pos, r)) {
				r += radius_step; 
				ray = cast_ray((theta + angle) * SKC::Numbers::DEG_TO_RAD, r, start_pos);

			}
			if (r > .0001) {
				points.push_back(ray); 
			}
		}
		return points;
	}
}