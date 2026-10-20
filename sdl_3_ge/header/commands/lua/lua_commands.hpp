#pragma 

#include "../command_buffer.hpp"
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
#include <format>
namespace skc::lua_statics {
	static int set_draw_color(lua_State* L) {
		int r = luaL_checknumber(L, -3);
		int g = luaL_checknumber(L, -2);
		int b = luaL_checknumber(L, -1);
		auto command = std::format("seti draw_color {} {} {}", r, g, b);
		command_buffer.add_command(command);
		return 0;
	}
	static int add_command(lua_State* L) {
		std::string cmd = luaL_checkstring(L, -1);
		command_buffer.add_command(cmd);
		return 0;
	}
	static int clear(lua_State* L) {
		command_buffer.add_command("clear");
		return 0;
	}

}