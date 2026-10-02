#pragma once 
#include "./Vector.hpp"
#include "./numbers.hpp"
#include <vector>
#include <functional>
namespace SKC::Math::Ray {
	template <std::floating_point T> 
	using ray_cast_pred = std::function<bool(Vect2<T>, Vect2<T>, T)>;
	
	template <std::floating_point T>  
	Vect2<T> cast_ray(T theta, T r, Vect2<T> start_pos = { 0,0 }) {
		Vect2<T> a = Vect2<T>::from_polar(theta, r);
		return a + start_pos; 
	}
	
	template <std::floating_point T>
	std::vector<Vect2<T>>
	cast_rays(
		T fov,
		T angle, 
		T radius_step,
		T angle_steps,  
		Vect2<T> start_pos,
		ray_cast_pred<T> pred
	) {
		std::vector<Vect2<T>> points = {};
		for (T theta = -(fov / 2); theta <= fov / 2; theta += fov / angle_steps) {
			double r = 0; 
			auto ray = cast_ray<T>((theta + angle) * SKC::Numbers::DEG_TO_RAD, r, start_pos); 
			while (pred(ray, start_pos, r)) {
				r += radius_step; 
				ray = cast_ray<T>((theta + angle) * SKC::Numbers::DEG_TO_RAD, r, start_pos);

			}
			if (r > .0001) {
				points.push_back(ray); 
			}
		}
		return points;
	}
}