#pragma once
#include <charconv>
#include <string>
namespace skc::util {
	template <typename T>
	T to_number(const std::string str) {
		T v; 
		auto f = str.c_str(), l = f + str.size();
		std::from_chars(f, l, v);
		return v; 
	}
}