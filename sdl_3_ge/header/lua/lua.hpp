#pragma once
#include <deque>
#include <string>
#include <filesystem>
#include <vector>
#include <ranges>
#include <string_view>
#include <format>
#include <optional>
#include <thread>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}
#include "../thread/smart_lock.hpp"

#include "./statics.hpp"
#include "./lua_type.hpp"
namespace skc {
	
	class lua_engine {
		std::mutex m_state_mutex; 
		lua_State* m_state;
		bool m_errored = false; 
	public: 
		lua_engine() {
			m_state = luaL_newstate();
			luaL_openlibs(m_state); 
		}
		~lua_engine() {
			if (m_state) {
				lua_close(m_state); 
			}
		}
		void lock() {
			m_state_mutex.lock(); 
		}
		void unlock() {
			m_state_mutex.unlock();
		}
		void create_table() {
			lua_newtable(m_state);
		}
		bool is_nil(int pos = -1) {
			return lua_isnil(m_state, pos); 
		}
		void append_table(lua_generic  name, lua_generic value) {
			if (!lua_istable(m_state, -1)) {
				std::println("NOT A TABLE"); 
				return; 
			}
			push(name);
			push(value); 
			lua_settable(m_state, -3); 
		}
		void append_table(const char* name, lua_generic value) {
			append_table(std::string(name), value); 
		}
		bool errored() const { return m_errored; }
		void push(lua_generic value) {
			if (auto n = value.as<lua_Number>(); n) {
				lua_pushnumber(m_state, *n);
				return; 
			}
			if (auto n = value.as<lua_Integer>(); n) {
				lua_pushinteger(m_state, *n);
				return;
			}
			if (auto n = value.as<std::string>(); n) {
				lua_pushstring(m_state, (*n).c_str());
				return;
			}
			if (auto n = value.as<bool>(); n ) {
				lua_pushboolean(m_state, *n);
				return;
			}
		}
		void load_lib(std::string name, const luaL_Reg functions[]) {
			luaL_newlib(m_state, functions); 
			
			lua_setglobal(m_state, name.c_str()); 
		}

		void pstack() {
			lua_statics::pstack(m_state); 
		}
		void get_global(std::string name) {
			lua_getglobal(m_state, name.c_str()); 
		}
		void set_global(std::string name) {
			lua_setglobal(m_state, name.c_str());
		}

		void set_global(const char* name) {
			lua_setglobal(m_state, name);
		}
		void set_global(std::string name, lua_generic val) {
			push(val); 
			set_global(name); 
		}
		void do_file(std::filesystem::path file) {
			auto e = luaL_dofile(m_state, file.string().c_str());
			if (e != LUA_OK) {
				std::println("{}", luaL_checkstring(m_state, -1));
				m_errored = true;
			}
		}
		void call(int nargs,int nresults = 1) {
			int e = lua_pcall(m_state, nargs, nresults,NULL);
			if (e != LUA_OK) {
				std::println("{}", luaL_checkstring(m_state, -1)); 
				lua_statics::pstack(m_state); 
				m_errored = true; 
			}
		}
		void reg(std::string function_name, lua_CFunction function) {
			lua_register(m_state, function_name.c_str(), function);

		}
		std::optional<lua_generic> at(int s) {
			auto typid = lua_type(m_state, s); 
			switch (typid) {
				case LUA_TBOOLEAN :{ 
					if (lua_toboolean(m_state, s)) return true; 
					return false; 
				}
				case LUA_TNUMBER: {
					return lua_tonumber(m_state, s); 
				}
				case LUA_TSTRING: {
					return std::string(lua_tostring(m_state, s));
				}
				default:return std::nullopt;
			}
		}
	};
}