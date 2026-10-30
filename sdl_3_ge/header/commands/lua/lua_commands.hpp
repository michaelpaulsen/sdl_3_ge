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
		auto l_top = lua_gettop(L);
		if (l_top < 3) return 0;

		int r = luaL_checknumber(L, 1);
		int g = luaL_checknumber(L, 2);
		int b = luaL_checknumber(L, 3);
		
		auto command = std::format("seti draw_color {} {} {}", r, g, b);
		if (l_top == 4) {
			command += std::format(" {}", luaL_checknumber(L, 4));
		}
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
	int bind(lua_State* L) {
		auto t = lua_gettop(L); 
		if (t < 2) { return 0;}
		if (lua_type(L, 2) != LUA_TSTRING) { return 0;}
		auto command = std::string("bind"); 
		unsigned kcode = 0; 
		auto kcode_t = lua_type(L, 1); 
		if (!(kcode_t == LUA_TSTRING || kcode_t == LUA_TNUMBER)) { return 0;  }
		if (kcode_t == LUA_TSTRING) {
			auto a = lua_tostring(L, 1); 
			if (strlen(a) == 1) {
				command_buffer.add_command(std::format("{} {:d} {}", command, a[0], lua_tostring(L,2)));
				return 0;
			}
			//TODO(skc) : make this parse special key names. 
			return 0; 
		}
		command_buffer.add_command(std::format("{} {} {}", command, lua_tonumber(L,1), lua_tostring(L, 2)));

	}

}