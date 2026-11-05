#pragma once
#include <vector>

#include "../command.hpp"
#include "../command_state.hpp"

#include "../../GE/Window.hpp"
#define PRIV_CHECK_COLOR() auto args = command.args();\
if (args < 3 or args > 4) return COMMAND_STATE::INVALID
namespace skc::commands {
	COMMAND_STATE set_draw_color(command& command, SKC::GE::window& window ) {
		PRIV_CHECK_COLOR();
		auto r = std::stoi(command.consume());
		auto g = std::stoi(command.consume());
		auto b = std::stoi(command.consume());
		unsigned char a = 255;
		if (command.args() >= 1) {
			a = std::stoi(command.consume());

		}
		window.set_draw_color(r, g, b, a);
		return COMMAND_STATE::PARSED;
	}
	COMMAND_STATE set_clear_color(command& command, SKC::GE::window& window) {
		PRIV_CHECK_COLOR();

		auto r = std::stoi(command.consume());
		auto g = std::stoi(command.consume());
		auto b = std::stoi(command.consume());
		unsigned char a = 255;
		if (command.args() >= 1) {
			a = std::stoi(command.consume());

		}
		window.set_background_color(r, g, b, a);
		return COMMAND_STATE::PARSED;
	}
}