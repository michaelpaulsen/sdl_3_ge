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
		auto d2r = SKC::Numbers::DEG_TO_RAD; 
		std::vector<Vect2d> points = {}; 
		auto rangle = (angle * Numbers::DEG_TO_RAD); 
		auto r = 0;
		auto cray = cast_ray(angle * d2r, r, start_pos); 

		while (pred(cray, start_pos, r)) {
			r += radius_step;
			cray = cast_ray((angle) * d2r, r, start_pos);

		}
		if (r > 0) points.push_back(cray);

		
		
		for (
			double theta = fov / (angle_steps / 2);
			theta < fov / 2;
			theta += fov / (angle_steps/2)
		) {
			r = 0; 
			auto lray = cast_ray(((-theta) + angle) * d2r, r, start_pos);
			while (pred(lray, start_pos, r)) {
				r += radius_step; 
				lray = cast_ray(((-theta) + angle) * d2r, r, start_pos);

			}
			if (r > 0) points.push_back(lray); 
			r = 0; 
			auto rray = cast_ray(((theta) + angle) * d2r, r, start_pos);
			while (pred(rray, start_pos, r)) {
				r += radius_step;
				rray = cast_ray(((theta) + angle) * d2r, r, start_pos);

			}
			if (r > 0) points.push_back(rray);

		}
		return points;
	}
}