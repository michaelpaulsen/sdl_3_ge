#pragma once
#include "lua.hpp"
#include "lualib.h"
#include <functional>
#include <string>
#include <filesystem>
#include <expected>

namespace SKC::lua{
	namespace fs = std::filesystem; 
	class Lua {
	using Lstate_t = lua_State*;
	Lstate_t m_lua_state; 
	void check_state(bool rr) const{
		if (!rr) {
			luaL_error(m_lua_state, "Error: %s\n", lua_tostring(m_lua_state, -1));
		}
	}
	public:
		Lua() {
			m_lua_state = luaL_newstate(); 
			luaL_openlibs(m_lua_state);
		}
		~Lua() {
			lua_close(m_lua_state);
		}
		
		//NOTE(skc) : the Lua state should be a singleton more or less. 
		//if you need to make your own lua state 
		Lua(Lua&) = delete; 
		Lua(Lua&&) = delete ;
		
		Lua& operator=(Lua&)  = delete;
		Lua& operator=(Lua&&) = delete;
		
		template<typename T>
		std::expected<T, size_t> pop() {

			//TODO(skc) implement me!
			return std::unexpected(0);
		} 

		template<typename T>
		bool push() {
			return false; 

		}

		bool execute_file(fs::path path) {
			luaL_dofile(m_lua_state, path.generic_string().c_str()); 
			//TODO(skc) implement me!
			return false; 
			}
		bool execute_command(std::string cmd) const {
			auto rr = luaL_dostring(m_lua_state, cmd.c_str());
			check_state(rr); 
			return rr;
		}
		
		void register_function(std::string name, lua_CFunction func) const {
			lua_register(m_lua_state, name.c_str(), func);
		}

	public:
	};
}