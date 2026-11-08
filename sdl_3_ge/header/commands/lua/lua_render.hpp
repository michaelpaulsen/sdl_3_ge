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
		
		if (stack_top < 1) {
			return 0;
		}
		std::string command = "draw texture";
		switch (lua_type(L, 1)) {
			case LUA_TSTRING:
			{
				command += std::format(" {}", luaL_checkstring(L, 1));
				break; 
			}
			case LUA_TNUMBER: {
				command += std::format(" {}", luaL_checknumber(L, 1));
				break;
			}
			default:return 0; 
		}
		switch (stack_top) {
		case 3: {
			float x = luaL_checknumber(L, 2);
			float y = luaL_checknumber(L, 3);
			command += std::format("{} {}", x, y);
			break;

		}
		case 4: {
			float x = luaL_checknumber(L, 2);
			float y = luaL_checknumber(L, 3);
			float s = luaL_checknumber(L, 4);
			command += std::format(" {} {} {} {}", x, y, s, s);
			break;
		}
		case 5: {
			float x = luaL_checknumber(L, 2);
			float y = luaL_checknumber(L, 3);
			float w = luaL_checknumber(L, 4);
			float h = luaL_checknumber(L, 5);
			command += std::format(" {} {} {} {}", x, y, w, h);
			break;
		}
		case 9: {
			int pos = 1; 
			float x       = luaL_checknumber(L, 2); 
			float y       = luaL_checknumber(L, 3);	
			float w       = luaL_checknumber(L, 4);	
			float h       = luaL_checknumber(L, 5);	
			float atlas_x = luaL_checknumber(L, 6);	
			float atlas_y = luaL_checknumber(L, 7);	
			float atlas_w = luaL_checknumber(L, 8);	
			float atlas_h = luaL_checknumber(L, 9);	
			
			command += std::format(" {} {} {} {}", x, y, w, h);
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