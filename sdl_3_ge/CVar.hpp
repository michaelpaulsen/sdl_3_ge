#pragma once
#include <string> 
#include <any>
#include <optional>
#include <vector> 
#define static_if if constexpr 

namespace SKC::GE {

	using cvar_value_t = std::string;
	class C_var {
		bool m_is_set = false; 
		std::string m_name; 
		cvar_value_t m_value{};
		public: 
			C_var(std::string name, cvar_value_t value) noexcept : m_is_set(true), m_name(name), m_value(value) {};
			C_var(std::string name) : m_name(name) {}; 
			auto name() const noexcept {
				return m_name; 
			}
			void set_value(cvar_value_t val) noexcept {
				m_is_set = true; 
				m_value = val;
			}
//NOTE(skc) : I don't think I am going to like this syntax but 
//I don't know a better way of doing this. 
//as<T>() just seems gross...  would much prefer as(T)..  
//the reason that I am making it so it returns an empty optional instead of 
//throwing to keep the logic code more clean.
// it also helps to abstract away the difference between the C_var being set or not. 

			template<typename T> 
			[[nodiscard]] std::optional<T> as() const noexcept {
				if (!m_is_set) return {}; 
				 
				static_if (std::is_integral_v<T>  && !std::is_same_v<T,bool>) {
					return static_cast<T>(std::strtol(m_value.c_str(), nullptr, 10));
				}else
				static_if (std::is_same_v<T, float>) {
					return std::strtof(m_value.c_str(), nullptr);
				}else
				static_if(std::is_same_v<T, double>) {
					return std::strtod(m_value.c_str(), nullptr);
				}
				else static_if (std::is_same_v<T,bool>){
					if (m_value == "false" || m_value == "0" || m_value.empty()) return { false }; else
					return { true };
				}else static_if(std::is_same_v<T, std::string>) {
					return m_value; 
				} else {
					return{};
				}
				
		}
	};
	class C_var_list {
		std::vector<C_var> m_c_vars; 
	public : 
		C_var_list() = default; 
		bool has(std::string name ) {
			for (const auto& cv : m_c_vars) {
				if (cv.name() == name) return true; 
			}
			return false; 
		}
		template<typename T> std::optional<T>  get_as(std::string name) {
			for (const auto& cv : m_c_vars) {
				if (cv.name() == name) return cv.as<T>(); 
			}
			return {}; 
		}
		template <typename T> T get_as(std::string name, T df) {
			for (const auto& cv : m_c_vars) {
				if (cv.name() == name) {
					if(cv.as<T>().has_value()) return cv.as<T>().value();
					return df; 
				}
			}
			return df;
		}
		void emplace_back(std::string name) {
			m_c_vars.emplace_back(name);

		}
		void emplace_back(std::string name, cvar_value_t value) {
			m_c_vars.emplace_back(name, value); 
		}
		auto size() {
			return m_c_vars.size(); 
		}
		bool set(std::string name, cvar_value_t value) {
			for (auto& cv : m_c_vars) {
				if (cv.name() == name) {
					cv.set_value(value);
					return true;
				}
			}
			return false; 
		}
	};

}

