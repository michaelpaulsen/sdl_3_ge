#pragma once 
#include "../command_buffer.hpp"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <format>
namespace skc::lua_statics {
	int tile_texture(lua_State* L ) {
		auto top = lua_gettop(L); 
		switch (top) {
			case 1: {
				command_buffer.add_command(std::format("tile {}", lua_tointeger(L, 1)));
				return 0; 
			}
			case 2: {
				auto tid = lua_tointeger(L, 1);
				auto scale = lua_tointeger(L, 2);
				command_buffer.add_command(std::format("tile {} {}", tid, scale));
				return 0;

			}
			case 4: {
				int t = 0; 
				auto tid = lua_tointeger(L, ++t);
				auto scale = lua_tointeger(L, ++t);
				auto x = lua_tointeger(L, ++t);
				auto y = lua_tointeger(L, ++t);
				command_buffer.add_command(
					std::format("tile {} {} {} {}", 
						tid,
						scale,
						x,y 
					)
				);
				return 0;

			}
			case 6: {
				int t = 0;
				auto tid = lua_tointeger(L, ++t);
				auto scale = lua_tointeger(L, ++t);
				auto x = lua_tointeger(L, ++t);
				auto y = lua_tointeger(L, ++t);
				auto w = lua_tointeger(L, ++t);
				auto h = lua_tointeger(L, ++t);
				command_buffer.add_command(
					std::format("tile {} {} {} {} {} {}",
						tid,
						scale,
						x, y, 
						w, h
					)
				);
				return 0;

			}
			default: {
				return 0;
			}
		}

	}


}