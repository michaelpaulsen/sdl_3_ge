#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include <format>
#include <optional>
#include <mutex> 


#include "../strmanip.hpp"
#include "../concepts.hpp"

namespace SKC::INI
{
	namespace _priv {
		template <SKC::concepts::number type_name>
		type_name string_to_number(std::string str) {
			if constexpr (std::is_same_v<type_name, int>) {
				return std::stoi(str);
			}
			else if constexpr (std::is_same_v<type_name, unsigned int>) {
				return std::stoul(str);
			}
			else if constexpr (std::is_same_v<type_name, long>) {
				return std::stol(str);
			}
			else if constexpr (std::is_same_v<type_name, unsigned long>) {
				return std::stoul(str);
			}
			else if constexpr (std::is_same_v<type_name, long long>) {
				return std::stoll(str);
			}
			else if constexpr (std::is_same_v<type_name, unsigned long long>) {
				return std::stoull(str);
			}
			else if constexpr (std::is_same_v<type_name, float>) {
				return std::stof(str);
			}
			else if constexpr (std::is_same_v<type_name, double>) {
				return std::stod(str);
			}
			else if constexpr (std::is_same_v<type_name, long double>) {
				return std::stold(str);
			}
		}


	}

	class settings_t {
		std::unordered_map<std::string, std::unordered_map<std::string, std::string> > m_settings_map;
		std::filesystem::path m_path{};
		std::mutex m_mutex;
		void parse_file() {
			{
				std::lock_guard lock{ m_mutex };
				m_settings_map.clear();
			}
			auto file_contents = SKC::read_whole_file(m_path);
			if (!file_contents) {
				throw std::runtime_error(std::format("ERROR UNABLE TO OPEN SETTINGS FILE {}", m_path.string()));
			}
			auto contents = *file_contents;
			std::string section{};
			for (const auto& a : std::ranges::to<std::vector<std::string>>(SKC::split(contents, '\n'))) {
				//TODO(skc) : this should warn if not empty... 
				
				if (a[0] == '[') {
					section = std::ranges::to<std::string>(
						a
						| std::views::drop_while([](const char t) {
							return std::isspace(t);  })
						| std::views::drop(1)
						| std::views::reverse
						| std::views::drop_while([](const char t) {
							return std::isspace(t);  })
						| std::views::drop(1)
						| std::views::reverse
					);
					continue;
				}
				if (not a.contains('=')) {
					std::println("[WARNING] line {} isnt a key value pair", a); 
					continue;
				}
				auto key = std::ranges::to<std::vector<std::string>>(
					a
					| std::views::split('=')
					| std::views::take(1)
				)[0];
				key = SKC::trim(key);
				auto value = std::ranges::to<std::string>(
					a
					| std::views::split('=')
					| std::views::drop(1)
					| std::views::join_with('=')
				);
				if (!value.empty()) {
					value = SKC::trim(value);

				}
				{
					std::lock_guard lock{ m_mutex };
					m_settings_map[section][key] = value;
				}
			}
		}
	public:
		auto& get_path() noexcept {
			return m_path;
		}
		auto const& get_path() const noexcept {
			return m_path;
		}
		auto get_map() const noexcept {
			return m_settings_map;
		}
		explicit settings_t(std::filesystem::path path) : m_path(std::move(path)) {
			parse_file();
		}


		settings_t() = delete;
		settings_t(const settings_t& other) = delete;
		settings_t(settings_t&& other) noexcept = delete;

		~settings_t() = default;
		void reload() {
			parse_file();
		}

#ifndef __INTELLISENSE__ 
		std::string& operator[](const std::string& section, std::string key) noexcept {
			return m_settings_map[section][key];
		}
#endif
		template <SKC::concepts::boolalnum T>
		T get(std::string section, std::string key) const {
			std::string m_value = m_settings_map.at(section).at(key);
			if constexpr (SKC::type_traits::is_number_v<T>) {
				return _priv::string_to_number<T>(m_value);
			}
			else if constexpr (std::is_same_v<T, std::string>) {
				return m_value;
			}
			else if constexpr (std::is_same_v<T, bool>) {
				if (m_value == "false") return false;
				return true;
			}
		}

		template <SKC::concepts::boolalnum T>
		T or_else(std::string section, std::string key, const T default_value) const {
			if (!m_settings_map.contains(section)) {
				return default_value;
			}

			if (!m_settings_map.at(section).contains(key)) {
				return default_value;
			}
			return get<T>(section, key);
		}

		auto at(std::string section) const {
			return m_settings_map.at(section);
		}
		auto at(std::string section, std::string key) const {
			return (m_settings_map.at(section)).at(key);
		}
		auto begin() noexcept {
			return m_settings_map.begin();
		}
		auto cbegin() const noexcept {
			return m_settings_map.cbegin();
		}

		auto end() noexcept {
			return m_settings_map.end();
		}
		auto cend() const noexcept {
			return m_settings_map.cend();
		}



		void save() {
			//TODO(skc): implement saving the settings back to the ini file
		}
	};
	
}
