#pragma once
#include <concepts>
#include <cmath> 

namespace SKC::Math {
template <typename T> concept numeber = std::is_integral_v<T> || std::is_floating_point_v<T>; 
	template<std::floating_point T> 
	struct Vect2 {
		T x{}, y{};
		
		Vect2(T _x, T _y) : x(_x), y(_y) {}; 
		T len() {
			return std::sqrt((x * x) + (y * y)); 
		}
		T len2() {
			return (x * x) + (y * y);
		}
		T dot(Vect2<T> other) {
			return (x * other.x) + (y * other.y); 
		}
		Vect2<T> operator + (Vect2<T> other) {
			return { x + other.x, y + other.y };
		}
		Vect2<T> operator - (Vect2<T> other) {
			return { x - other.x, y - other.y }; 
		}
		Vect2<T> operator * (numeber auto scalar) {
			return { x * scalar, y * scalar }; 
		}
	};
}