#pragma once
#include "../math/Vector.hpp"
#include "./parsers/create_command.hpp"
#include "./command_state.hpp"
#include "../lua/lua.hpp"
#include "../GE/Window.hpp"
#include "command.hpp"
namespace skc::commands {
	COMMAND_STATE parse_init(
		lua_engine& lua,
		SKC::GE::window& window,
		command& command,
		const skc_Vect2i& window_size
	) {

		auto cmd = command.cmd();
		if (cmd == "create") {
			auto subcommand = command.consume();
			if (subcommand == "texture") {
				return commands::create_texture(command, lua, window, window_size);
			}
		}
		if (cmd == "pixlize") {
			//TODO(SKC) implement parsing texture names. 
			return COMMAND_STATE::NOT_IMPLEMENTED;
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

