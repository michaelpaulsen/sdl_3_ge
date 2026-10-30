#pragma once
#include <vector>

#include "../command.hpp"

#include "../../GE/Window.hpp"
#include "../../GE/font_options.hpp"

namespace skc::commands {
	bool seti_command(SKC::GE::window& window,
		skc::command& command,
		SKC::GE::font_options& fo 
	) {
		auto sub_command = command.consume();
		if (sub_command == "draw_color") {
			auto r = std::stoi(command.consume());
			auto g = std::stoi(command.consume());
			auto b = std::stoi(command.consume());
			unsigned char a = 255;
			if (command.args() >= 1) {
				a = std::stoi(command.consume());
				
			}
			window.set_draw_color(r, g, b,a);
			return true;
		}
		if (sub_command == "clear_color") {
			auto r = std::stoi(command.consume());
			auto g = std::stoi(command.consume());
			auto b = std::stoi(command.consume());
			unsigned char a = 255;
			if (command.args() >= 1) {
				a = std::stoi(command.consume());

			}
			window.set_background_color(r, g, b,a);
			return true;
		}
		if (sub_command == "font_color") {
			auto r = std::stoi(command.consume());
			auto g = std::stoi(command.consume());
			auto b = std::stoi(command.consume());
			fo.color.r = r;
			fo.color.g = g;
			fo.color.b = b;
			return true;
		}
		if (sub_command == "render_target") {
			auto r = std::stoi(command.consume());

			window.set_render_target(r);
			return true;
		}
	}
}