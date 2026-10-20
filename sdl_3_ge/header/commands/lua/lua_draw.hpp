#pragma once
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}
#include <format>
namespace skc::lua_statics {
	static int draw_rect(lua_State* L) {
		int l_top = lua_gettop(L);
		if (l_top == 4) {
			float x = luaL_checknumber(L, -4);
			float y = luaL_checknumber(L, -3);
			float w = luaL_checknumber(L, -2);
			float h = luaL_checknumber(L, -1);
			command_buffer.add_command(std::format("draw rect {} {} {} {}", x, y, w, h));
			return 0;
		}
		if (l_top == 3) {
			float x = luaL_checknumber(L, -3);
			float y = luaL_checknumber(L, -2);
			float s = luaL_checknumber(L, -1);
			command_buffer.add_command(std::format("draw rect {} {} {} {}", x, y, s, s));
			return 0;
		}
		return 0;
	}
}