#pragma once
#include <concepts>
#include <cmath> 

namespace SKC::Math {
template <typename T> concept numeber = std::is_integral_v<T> || std::is_floating_point_v<T>; 
	template<numeber T>
	struct Vect2 {
		T x{}, y{};
		
		Vect2(T _x, T _y) : x(_x), y(_y) {}; 
		static Vect2<T>  from_polar(double angle, double length = 1) {
			return{ static_cast<T>(std::sin(angle) * length), static_cast<T>(-std::cos(angle) * length) };
		}
		double len() {
			return std::sqrt((x * x) + (y * y)); 
		}
		T len2() {
			return (x * x) + (y * y);
		}
		void normalize() {
			double l = len();
			x /= l;
			y /= l; 
		}
		T dot(Vect2<T> other) {
			return (x * other.x) + (y * other.y); 
		}
		void operator+=(T other) {
			x += other;
			y += other;
		}
		void operator+=(Vect2<T> other) {
			x += other.x; 
			y += other.y; 
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
using skc_Vect2d = SKC::Math::Vect2<double>;
using skc_Vect2f = SKC::Math::Vect2<float>;
using skc_Vect2i = SKC::Math::Vect2<int>;