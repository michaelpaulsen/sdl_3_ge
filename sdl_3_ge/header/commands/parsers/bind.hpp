#pragma once
#include "../command.hpp"
#include <string>
/**/

namespace skc::commands {
	bool bind(skc::command command, std::unordered_map<unsigned, std::string>& kmap) {
		auto kcode = skc::util::to_number<unsigned>(command.consume());
		
		std::string kcommand = "nil";
		if (command.args() >= 1) { kcommand = command.consume(); }
		kmap[kcode] = kcommand;
		return true;
	}
}