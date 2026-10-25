#pragma once
#include <string>
#include <optional>
#include "../util.hpp"
namespace skc::commands {
	std::optional<int> get_key_index(std::string key) {
		if (key.size() == 1 and
			(key[0] >= 'a' and key[0] <= 'z')) {
			return key[0];
		}
		else {
			try{
			return skc::util::to_number<int>(key);
			}
			catch (...) {
				return std::nullopt; 
			}
		}
		
	}
}