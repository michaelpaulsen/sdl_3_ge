#pragma once
#include <type_traits>
#include <string>
#include <variant>
#include <optional>


extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}
namespace skc {
	namespace concepts {
		template <typename T> concept c_lua_type =
			std::is_same_v< lua_Integer, T> ||
			std::is_same_v< lua_Number, T> ||
			std::is_same_v< bool, T> ||
			std::is_same_v< std::string, T>;
	}
	class lua_generic {
		using T = std::variant< lua_Integer, lua_Number, bool, std::string>;
		T m_value;
	public:

		lua_generic(std::string v) : m_value(v) {}
		lua_generic(lua_Integer v) : m_value(v) {}
		lua_generic(lua_Number v) : m_value(v) {}
		lua_generic(bool v) : m_value(v) {}
		template<concepts::c_lua_type T>
		bool is() {
			return std::holds_alternative<T>(m_value);
		}
		template<concepts::c_lua_type T>
		std::optional<T> as() {
			if (std::holds_alternative<T>(m_value)) {
				return std::get<T>(m_value);
			}
			return std::nullopt;
		}

	};
}