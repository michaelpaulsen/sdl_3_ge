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
		using lua_funct_t = std::function<int(Lstate_t)>;
		Lua() {
			m_lua_state = luaL_newstate(); 
		}
		~Lua() {
			lua_close(m_lua_state);
		}
		void register_function(std::string name, lua_CFunction func ) {
			lua_register(m_lua_state, name.c_str(), func);

		}

	public:
	};
}