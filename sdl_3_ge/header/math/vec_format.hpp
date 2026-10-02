#pragma once


#include "Vector.hpp"
#include <format>
#include <typeinfo>
#include <span>

#define fmt_type SKC::Math::Vect2<T>
template <typename T>
struct std::formatter<fmt_type> {
	bool show_type = false;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		while (pos != ctx.end() && *pos != '}') {
			show_type = (*pos == '#') || show_type;
			++pos;
		}
		return pos;
	}

	auto format(const fmt_type& obj, std::format_context& ctx) const {

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
#undef fmt_type

#define fmt_type std::span<SKC::Math::Vect2<T>>
template <typename T>
struct std::formatter<fmt_type> {
	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	auto format(const fmt_type& obj, std::format_context& ctx) const {

		std::string fmt_string = "[";
		bool first = true;
		for (const auto& point : obj) {
			if (not first) fmt_string += ",";
			fmt_string += std::format("{}", point);
			first = false;
		}
		fmt_string += "]";

		return std::format_to(ctx.out(), "{}", fmt_string);
	}

};
#undef fmt_type