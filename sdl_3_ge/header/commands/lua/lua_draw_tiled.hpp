#pragma once
#include "../command_buffer.hpp"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <format>

namespace skc::lua_statics {
	draw_tiled(lua_State* L) {
		int l_top = lua_gettop(L);
		int lua_indx = 1; 
		if (l_top == 0 ) return 0;
		if (l_top > 8) l_top = 8; 
		std::string command = std::format("draw tiled {}", lua_tointeger(L, lua_indx++));
		while (lua_indx < l_top) {
			if ((l_top - lua_indx) < 2) break;
			int lua_next_idx = lua_indx + 1; 
			if (!lua_isnumber(L, lua_indx)) { return 0; }
			if (!lua_isnumber(L, lua_next_idx)) { return 0; }
			command = std::format(" {} {}",
				lua_tointeger(L, lua_indx),
				lua_tointeger(L, lua_next_idx)
			);
			lua_indx += 2; 
		}

	}
}