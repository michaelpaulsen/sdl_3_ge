#pragma once
#include <unordered_map>
#include <string>
namespace skc::commands::types {
	using key_bind_t = std::unordered_map<unsigned, std::string>; 
	using tmap_t = std::unordered_map<std::string, size_t>; 
}