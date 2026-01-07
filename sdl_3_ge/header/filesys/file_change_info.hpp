#pragma once 
#include <filesystem>
#include <cstdint>
namespace SKC::file_api {

	enum FILE_ACTION :uint64_t;

	struct fs_change_info_t {
		std::filesystem::path fname{}, old_name{};
		FILE_ACTION action;

	};
}