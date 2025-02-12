#pragma once
#include "lua.hpp"
#include "lualib.h"
#include <functional>
#include <string>
namespace SKC::lua{
	class Lua {
	using Lstate_t = lua_State*;
	Lstate_t m_lua_state; 
	public:
		Lua() {
			m_lua_state = luaL_newstate(); 
			luaL_openlibs(m_lua_state);
		}
		~Lua() {
			lua_close(m_lua_state);
		}
		
		//NOTE(skc): DO NOT COPY THE LUA ENVORMENT YOU ARE ASKING FOR A DOUBLE 
		// FREE IF YOU DO  
		Lua(Lua&) = delete; 
		Lua(Lua&&) = delete ;
		
		Lua& operator=(Lua&) = delete;
		Lua& operator=(Lua&&) = delete;
		bool execute_command() {
			const char* s = "print(\"[Lua] Hello from this string\")";
			auto rr = luaL_dostring(m_lua_state, s);
			if (rr != LUA_OK) {
				luaL_error(m_lua_state, "Error: %s\n", lua_tostring(m_lua_state, -1));

			}
			return rr == LUA_OK;
		}
		void register_function(std::string name, lua_CFunction func) {
			lua_register(m_lua_state, name.c_str(), func);
		}

	public:
	};
}