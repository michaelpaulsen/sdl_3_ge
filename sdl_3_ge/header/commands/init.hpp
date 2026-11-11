#pragma once
#include <unordered_map>


#include "../math/Vector.hpp"
#include "./parsers/create_command.hpp"
#include "./command_state.hpp"
#include "../lua/lua.hpp"
#include "../GE/Window.hpp"
#include "command.hpp"
#include "types.hpp"
namespace skc::commands {
	COMMAND_STATE parse_init(
		lua_engine& lua,
		SKC::GE::window& window,
		command& command,
		types::tmap_t& tnames,
		const skc_Vect2i& window_size
	) {

		auto cmd = command.cmd();
		if (cmd == "create") {
			auto subcommand = command.consume();
			if (subcommand == "texture") {
				return commands::create_texture(command, lua, window, tnames, window_size);
			}
		}
		if (cmd == "pixlize") {
			if (command.args() != 1) return COMMAND_STATE::INVALID; 
			try{
				auto name = command.consume(); 
				auto tid = tnames.at(name); 
				std::println("pixelizing {} (tid: {})", name, tid); 
				window.set_texture_scale_mode(tid, SDL_SCALEMODE_PIXELART);
				return COMMAND_STATE::PARSED;
			}
			catch (...) {
				//NOTE(SKC): no texture found
				return COMMAND_STATE::INVALID; 
			}
		}
		if (cmd == "load") {
			auto mode = command.consume();
			if (mode == "texture") {
				auto tx_path = command.consume();
				auto tx_name = command.consume();
				SKC::GE::color color = { 0,0,0 };
				long long tid = 1;
				if (command.args() >= 3) {
					color.r = skc::util::to_number<uint8_t>(command.consume());
					color.g = skc::util::to_number<uint8_t>(command.consume());
					color.b = skc::util::to_number<uint8_t>(command.consume());
					tid = window.create_texture(tx_path, color);

				}
				else {
					tid = window.create_texture(tx_path);
				}
				long long x = window.get_texture_width(tid);
				long long y = window.get_texture_height(tid);
				tnames[tx_name] = tid; 
				{
					auto lg = std::lock_guard(lua);
					lua.get_global(texture_array_name);
					lua.push(tx_name);
					lua.create_table();
					lua.append_table("tid", tid);
					lua.append_table("width", x);
					lua.append_table("height", y);
					lua.append_table("path", tx_path);
					//NOTE(Skc) : by this point the stack looks like 
					//[texture_array_name, tx_name, <TABLE>] 
					//this appends the above created table with the key 
					//tx_name to texture_array_name
					lua.append_table();
					lua.set_global(texture_array_name);

				}
				return COMMAND_STATE::PARSED;
			}
			return COMMAND_STATE::INVALID;
		}
		return COMMAND_STATE::CONTINUE;
	}
}

