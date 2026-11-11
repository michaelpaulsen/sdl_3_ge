#pragma once
#include <unordered_map>


#include "../lua/lua.hpp"
#include "../GE/Window.hpp"

#include "command.hpp"

#include "./parsers/draw.hpp"
#include "./parsers/set.hpp"


#include "command_state.hpp" 
#include "types.hpp"

namespace skc::commands {
	
	COMMAND_STATE parse_frame(
		skc::lua_engine& lua,
		SKC::GE::window& window,
		command& command,
		types::key_bind_t& kmap,
		const types::tmap_t & tmap
	) {

		auto cmd = command.cmd();
		if (cmd == "exit") {
			return COMMAND_STATE::EXIT;
		}
		if (cmd == "clear") {
			window.clear();
			return COMMAND_STATE::PARSED;
		}
		if (cmd == "bind") {
			auto kcode = skc::util::to_number<unsigned>(command.consume());

			if (command.args() != 1) { 
				return COMMAND_STATE::INVALID; 
			}
			kmap[kcode] = command.consume() ;
			return COMMAND_STATE::PARSED;
		}
		if (cmd == "unbind") {
			auto kcode = skc::util::to_number<unsigned>(command.consume());
			if (command.args() != 0) {
				return COMMAND_STATE::INVALID; 
			}
			kmap[kcode] = "nil";
			return COMMAND_STATE::PARSED;
		}

		if (cmd == "line") {
			if (command.args() != 4) return COMMAND_STATE::INVALID; 
			auto x1 = skc::util::to_number<float>(command.consume());
			auto y1 = skc::util::to_number<float>(command.consume());
			auto x2 = skc::util::to_number<float>(command.consume());
			auto y2 = skc::util::to_number<float>(command.consume());
			window.draw_line(x1, y1, x2, y2);
			return COMMAND_STATE::PARSED;
		}

		if (cmd == "tile") {
			return skc::commands::tile_command(window, command, tmap);
		}
		if (cmd == "draw") {
			auto sub_command = command.consume();
			if (sub_command == "rect") {
				size_t param_count = command.args();
				if (param_count == 4) {
					auto x = skc::util::to_number<double>(command.consume());
					auto y = skc::util::to_number<double>(command.consume());
					auto w = skc::util::to_number<double>(command.consume());
					auto h = skc::util::to_number<double>(command.consume());
					window.fill_rectangle(x, y, w, h);
					return COMMAND_STATE::PARSED;
				}
				return COMMAND_STATE::INVALID;
			}
			if (sub_command == "texture") {
				return render_texture(window, command, tmap);
			}
			return COMMAND_STATE::INVALID;
			
			//return skc::commands::draw_command(window, command,tmap);
		}

		if (cmd == "alpha") {
			if (command.args() != 2) return COMMAND_STATE::INVALID; 
			auto tid = skc::util::to_number<size_t>(command.consume());
			auto alpha = skc::util::to_number<uint8_t>(command.consume());
			window.set_texture_alpha_mod(tid, alpha);
			return COMMAND_STATE::PARSED;

		}
		if (cmd == "seti") {
			auto sub_command = command.consume();
			if (sub_command == "draw_color") {
				return skc::commands::set_draw_color(command, window); 
			}
			if (sub_command == "clear_color") {
				return skc::commands::set_clear_color(command, window); 
			}
			if (sub_command == "render_target") {
				auto r = std::stoi(command.consume());

				window.set_render_target(r);
				return COMMAND_STATE::PARSED;
			}
			return COMMAND_STATE::INVALID; 

		}
	}

}