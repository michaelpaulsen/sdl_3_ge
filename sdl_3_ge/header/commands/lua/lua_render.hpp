#pragma once

#include "../command_buffer.hpp"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}
#include <format>
namespace skc::lua_statics{
	static int render_texture(lua_State * L) {
		auto stack_top = lua_gettop(L);
		//if (stack_top == 0 || stack_top > 5) return 0; 
		std::string command = "draw texture ";
		switch (stack_top) {
		case 1: {
			float tid = luaL_checknumber(L, -1);
			command += std::format("{}", tid);
			break;

		}

		case 3: {
			int tid = luaL_checknumber(L, -3);
			float x = luaL_checknumber(L, -2);
			float y = luaL_checknumber(L, -1);
			command += std::format("{} {} {}", tid, x, y);
			break;

		}
		case 4: {
			int tid = luaL_checknumber(L, -4);
			float x = luaL_checknumber(L, -3);
			float y = luaL_checknumber(L, -2);
			float s = luaL_checknumber(L, -1);
			command += std::format("{} {} {} {} {}", tid, x, y, s, s);
			break;
		}
		case 5: {
			int tid = luaL_checknumber(L, -5);
			float x = luaL_checknumber(L, -4);
			float y = luaL_checknumber(L, -3);
			float w = luaL_checknumber(L, -2);
			float h = luaL_checknumber(L, -1);
			command += std::format("{} {} {} {} {}", tid, x, y, w, h);
			break;
		}
		case 9: {
			int pos = 0; 
			int tid = luaL_checknumber(L, ++pos);
			float x = luaL_checknumber(L, ++pos);
			float y = luaL_checknumber(L, ++pos);
			float w = luaL_checknumber(L, ++pos);
			float h = luaL_checknumber(L, ++pos);
			float atlas_x = luaL_checknumber(L, ++pos);
			float atlas_y = luaL_checknumber(L, ++pos);
			float atlas_w = luaL_checknumber(L, ++pos);
			float atlas_h = luaL_checknumber(L, ++pos);
			
			command += std::format("{} {} {} {} {}", tid, x, y, w, h);
			command += std::format(" {} {} {} {}",  
				atlas_x,
				atlas_y,
				atlas_w,
				atlas_h);
			break;
		}
		default: { return 0; }
		}

		command_buffer.add_command(command);
		return 0;
	}
}