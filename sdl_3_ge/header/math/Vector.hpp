#pragma once
#include <cmath> 
#include <format>
#include <typeinfo>
namespace SKC::Math {
	template <typename T> concept number = std::is_integral_v<T> || std::is_floating_point_v<T>;
	template<number T>
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

		//scalar operations
		void operator+=(number auto other) {
			x += other;
			y += other;
		}
		Vect2<T> operator * (number auto scalar) {
			return { x * scalar, y * scalar };
		}
		Vect2<T> operator / (number auto scalar) {
			return { x / scalar, y / scalar };
		}
	
		template<typename Y> 
		void operator+=(Vect2<Y> other) {
			x += other.x;
			y += other.y;
		}

		template<typename Y>
		Vect2<T> operator + (Vect2<Y> other) {
			return { x + other.x, y + other.y };
		}
		template<typename Y>
		Vect2<T> operator - (Vect2<Y> other) {
			return { x - other.x, y - other.y };
		}
		
		//NOTE(skc) : this is a template so that it 
		//behaves like normal math... 

		//TODO(skc) : name this template paramiter something better
		template<number Y>
		Vect2<T> operator/ (Vect2<Y> divisor) {
			return { x / divisor.x, y / divisor.y };
		}

		template<number U>
		explicit operator Vect2<U>() const {
			return { static_cast<U>(x), static_cast<U>(y) };
		}
	};
	using Vect2d = SKC::Math::Vect2<double>;
	using Vect2f = SKC::Math::Vect2<float>;
	using Vect2i = SKC::Math::Vect2<int>;
}
using skc_Vect2d = SKC::Math::Vect2d;
using skc_Vect2f = SKC::Math::Vect2f;
using skc_Vect2i = SKC::Math::Vect2i;

//TODO(SKC): move to own file.
// this should be in own file because there is no other reason that format is included in this file
// make this so that it only outputs in dbg mode... 
template <typename T>
struct std::formatter<SKC::Math::Vect2<T>> {
	bool show_type = false;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		while (pos != ctx.end() && *pos != '}') {
			show_type = (*pos == '#') || show_type;
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