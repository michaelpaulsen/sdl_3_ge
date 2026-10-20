#pragma once 

#include "../command_buffer.hpp"

#include <format>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

namespace skc::lua_statics {
	int command_ex(lua_State* L) {
		int top = lua_gettop(L);
		if (top < 1 or lua_type(L, 1) != LUA_TSTRING) { return 0; }
		if (top == 1) {
			command_buffer.add_command(lua_tostring(L, 1)); 
			return 0;

		}
		std::string command{ lua_tostring(L, 1) };
		command += " "; 
		for (int i = 2; i <= top; ++i) {
			int t = lua_type(L, i);
			switch (t) {
			case LUA_TSTRING: {
				command += std::format(R"("{}")", lua_tostring(L, i));
				break;
			}
			case LUA_TBOOLEAN: {
				command += std::format("{}", lua_toboolean(L, i));
				break;
			}
			case LUA_TNUMBER: {
				command += std::format("{}", lua_tonumber(L, i));
				break;
			}
			case LUA_TNIL: { command += "NILL"; break; }
			default:
				break;
			}
			if (i != top) command += " ";
		}
		command_buffer.add_command(command);
		return 0;
	}
}