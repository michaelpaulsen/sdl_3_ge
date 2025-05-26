#pragma once
#include <concepts>
#include <cmath> 
#include <format>
#include <typeinfo>
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


//TODO(SKC): move to own file.
// this should be in own file because there is no other reason that format is included in this file.
//TODO(SKC) : make this so that it only outputs in dbg mode... 
template <typename T>
struct std::formatter<SKC::Math::Vect2<T>> {
	bool show_type = false;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		while (pos != ctx.end() && *pos != '}') {
			show_type = (*pos == '#');
			++pos;
		}
		return pos;
	}

	auto format(const SKC::Math::Vect2<T>& obj, std::format_context& ctx) const {

		std::string fmt_string = "";
		if (show_type) {
			fmt_string += "vec2<";
			fmt_string += typeid(T).name();
			fmt_string += ">";

		}
		fmt_string += std::format("({}, {})", obj.x, obj.y);
		return std::format_to(ctx.out(), "{}", fmt_string);
	}

};