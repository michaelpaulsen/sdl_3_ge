#pragma once 
#include "../command_buffer.hpp"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <format>
namespace skc::lua_statics {
	int tile_texture(lua_State* L) {
		auto top = lua_gettop(L);
		if (top == 0) return 0;
		std::string cmd = "tile";
		switch (lua_type(L, 1)) {
		case LUA_TSTRING: {
			cmd += lua_tostring(L, 1); 
			break;
		}
		case LUA_TNUMBER: {
			cmd += std::format(" {}", lua_tonumber(L, 1)); 
			break; 
		}
			default: return 0; 
		}
		if (top == 1) {
			command_buffer.add_command(cmd); 
			return 0; 
		}
		if (top == 2) {
			command_buffer.add_command(cmd + std::format(" {}", luaL_checknumber(L, 2)));
			return 0;
		}
		cmd += std::format(" {}", luaL_checknumber(L, 2));
		//tid scale x1 y1 w1 h1 x2 y2 w2 h2
		if (top > 10) top = 10;
		int lua_index = 2;
		while ((top - lua_index) >= 2) {
			if ((top - lua_index) < 2) break;
			cmd += std::format(" {}", luaL_checknumber(L, ++lua_index));
			cmd += std::format(" {}", luaL_checknumber(L, ++lua_index));
		}
		command_buffer.add_command(cmd);
		return 0; 

	}
}