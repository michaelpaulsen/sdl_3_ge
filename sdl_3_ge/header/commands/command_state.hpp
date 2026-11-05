#pragma once
namespace skc {
	enum class COMMAND_STATE {
		EXIT,
		CONTINUE,
		INVALID,
		NOT_IMPLEMENTED, 
		PARSED,
	};

	bool should_exit(COMMAND_STATE s) {
		return s == COMMAND_STATE::EXIT;
	}
	bool has_parsed_command(COMMAND_STATE s) {
		return s == COMMAND_STATE::PARSED;
	}
	bool is_invalid(COMMAND_STATE s) {
		return s == COMMAND_STATE::INVALID;
	}
}