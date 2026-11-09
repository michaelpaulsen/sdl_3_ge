#pragma once

#include <mutex>
#include <unordered_map>

#include "../command.hpp"
#include "../../GE/Window.hpp"
#include "../../math/Vector.hpp"
#include "../../lua/lua.hpp"
#include "../../util.hpp"
#include "../command_state.hpp"


namespace skc::commands {
	namespace priv {
		using int64 = long long;
		void create_lua_texture_table(
			skc::lua_engine& lua,
			std::string name,
			int64 tid,
			int64 width,
			int64 height
		) {
			{
				auto lg = std::lock_guard(lua);
				lua.get_global(texture_array_name);
				lua.push(name);
				lua.create_table();
				lua.append_table("tid", tid);
				lua.append_table("width", width);
				lua.append_table("height", height);
				lua.append_table();
				lua.set_global(texture_array_name);
			}
		}
	}
	COMMAND_STATE create_texture(
		skc::command& command,
		skc::lua_engine& lua,
		SKC::GE::window& window,
		std::unordered_map<std::string, size_t>& tnames,
		const skc_Vect2i& window_size
	) {
		if (command.args() == 0 || command.args() > 3) return COMMAND_STATE::INVALID;
		auto name = command.consume();
		//NOTE(SKC) it is never not a bug to create two textures with the same name! 
		// since this is only ever done prior to loading, speed is not an issue.  
		if (tnames.contains(name)) return COMMAND_STATE::INVALID; 
		int x = window_size.x, y = window_size.y;
		//NOTE(SKC) by the time we get here we know that command.args()
		// is one of 0,1 or 2 (because the call to consume "removed" one) 
		// if it is 1 that means that only a width is passed this is invalid so we return invalid
		if (command.args() == 1) return COMMAND_STATE::INVALID;
		if (command.args() == 0) {
			auto tid = window.create_texture(); 
			priv::create_lua_texture_table(lua, name, window.create_texture(), x, y);
			tnames[name] = tid; 
			return COMMAND_STATE::PARSED;
		}
		//NOTE(SKC) by the time that we are here we know command.args() is 2 because 
		//both the 0 and 1 cases have already been handled. so this is safe 
		auto tid = window.create_texture(x, y); 
		x = skc::util::to_number<int>(command.consume());
		y = skc::util::to_number<int>(command.consume());
		tnames[name] = tid; 
		priv::create_lua_texture_table(lua, name, tid , x, y);
		return COMMAND_STATE::PARSED; 
	}
}
