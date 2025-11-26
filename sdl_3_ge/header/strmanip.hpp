#pragma once
#include <string>
#include <expected>
#include <filesystem>
#include <fstream>
#include <ranges>


namespace SKC{

	[[nodiscard]]  static std::expected<std::string, std::string>  read_whole_file(std::filesystem::path path) {
		auto input = std::ifstream(path);
		//TODO(skc): make this return expected.
		if (not input.is_open()) return std::unexpected("ERROR UNABLE TO OPEN FILE");
		std::ostringstream t;
		t << input.rdbuf();
		input.close();
		return t.str();

	}

	static std::string trim(std::string line) {
		constexpr char whitepsace[] = " \n\v\r\v\f";
		auto first_indx = line.find_first_not_of(whitepsace);
		if (first_indx == std::string::npos) {
			return ""; 
		}
		auto length = (line.find_last_not_of(whitepsace) - first_indx) + 1;
		return line.substr(first_indx, length);
	}
	static auto split(std::string line, const char delem) {
		return std::ranges::to<std::vector>(line
			| std::views::split(delem)
			| std::views::transform([](const auto& t) {
				return std::string(t.begin(), t.end());

				}));
	}
}