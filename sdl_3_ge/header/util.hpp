#pragma once
#include <charconv>
#include <string>
#include <algorithm>
namespace skc::util {
	bool is_num(std::string str) {
		bool ret = true;
		std::for_each(str.begin(), str.end(), [&ret](char &a) {
			ret = ((a <= '9' && a >= '0') || a == '.') && ret;
		}); 
		return ret;
	}

	template <typename T>
	T to_number(const std::string str) {
		T v; 
		auto f = str.c_str(), l = f + str.size();
		std::from_chars(f, l, v);
		return v; 
	}
}