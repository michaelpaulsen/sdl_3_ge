#pragma once
#include <unordered_map>
#include <ranges>


#include "../lua/lua.hpp"
#include "../GE/Window.hpp"

#include "command.hpp"

#include "./parsers/draw.hpp"
#include "./parsers/set.hpp"


#include "command_state.hpp" 
#include "types.hpp"

#include "../GE/types.hpp"
namespace skc::commands {
	unsigned get_key_code(
		command& command
	) {
		auto bind = command.consume() | std::ranges::views::transform([](char u) ->char {
			auto uu = tolower(u);
			if (uu > 0 and uu < 128) return (char)uu; 
			return u; 
		}) | std::ranges::to<std::string>();

		//NOTE(skc) : if the bind is a single char
		// we just want to set it to the unsigned repersentation of that. 

		if (bind.size() == 1) {
			return (unsigned)bind[0]; 
		}
		namespace skcGE_types = SKC::GE::types; 
		static constexpr auto INVALID_EID = static_cast<unsigned>(skcGE_types::E_EVENT_IDS::LAST);
		if (bind == "+m_left"  ) { return static_cast<unsigned>(skcGE_types::E_EVENT_IDS::MOUSE_LEFT_DOWN  ); }
		if (bind == "-m_left"  ) { return static_cast<unsigned>(skcGE_types::E_EVENT_IDS::MOUSE_LEFT_UP    ); }
		if (bind == "+m_right" ) { return static_cast<unsigned>(skcGE_types::E_EVENT_IDS::MOUSE_RIGHT_DOWN ); }
		if (bind == "-m_right" ) { return static_cast<unsigned>(skcGE_types::E_EVENT_IDS::MOUSE_RIGHT_UP   ); }
		if (bind == "+m_middle") { return static_cast<unsigned>(skcGE_types::E_EVENT_IDS::MOUSE_MIDDLE_DOWN); }
		if (bind == "-m_middle") { return static_cast<unsigned>(skcGE_types::E_EVENT_IDS::MOUSE_MIDDLE_UP  ); }

		if (bind.size() > 3) {
			return  INVALID_EID;
		}


		if (bind == "bsp"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::BACKSPACE); }
		if (bind == "rtn"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::RETURN   ); }
		if (bind == "ist"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::INSERT   ); }
		if (bind == "isr"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::INSERT   ); }
		if (bind == "hme"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::HOME     ); }
		if (bind == "end"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::END      ); }
		if (bind == "pgd"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::PAGEDOWN ); }
		if (bind == "pgu"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::PAGEUP   ); }
		if (bind == "up"       ) { return  static_cast<unsigned>(skcGE_types::special_keys::UP       ); }
		if (bind == "dwn"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::DOWN     ); }
		if (bind == "lft"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::LEFT     ); }
		if (bind == "rgt"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::RIGHT    ); }
		if (bind == "esc"      ) { return  static_cast<unsigned>(skcGE_types::special_keys::ESC      ); }
		
		//TODO(skc) support F13-F24. this is low proiority since no keyboards use them anyway. 
		
		//NOTE(skc) this assumes that F1 - F12 are in order that should never change so this is fine. 
		//By the time that we get here we know that bind.size() >= 2; that means that it is not just binding the F key. 
		if (bind[0] == 'f') {
		
			if (bind.size() == 3 ) {
				if (bind[1] == '1') {
					if (bind == "f10") return static_cast<unsigned>(skcGE_types::special_keys::F10);
					if (bind == "f11") return static_cast<unsigned>(skcGE_types::special_keys::F11);
					if (bind == "f12") return static_cast<unsigned>(skcGE_types::special_keys::F12);
				}
				return INVALID_EID; 
			}
			return static_cast<unsigned>(skcGE_types::special_keys::F1) + (bind[1] - '1');

		}
		return INVALID_EID;
	}

	COMMAND_STATE parse_frame(
		skc::lua_engine& lua,
		SKC::GE::window& window,
		command& command,
		types::key_bind_t& kmap,
		const types::tmap_t & tmap
	) {

		auto cmd = command.cmd();
		if (cmd == "exit") {
			return COMMAND_STATE::EXIT;
		}
		if (cmd == "clear") {
			window.clear();
			return COMMAND_STATE::PARSED;
		}
		if (cmd == "bind") {
			if (command.args() < 1) return COMMAND_STATE::INVALID; 
			auto kcode = get_key_code(command); 
			if (kcode == (unsigned)SKC::GE::types::E_EVENT_IDS::LAST) return COMMAND_STATE::INVALID; 
			if (command.args() != 1) { 
				return COMMAND_STATE::INVALID; 
			}
			kmap[kcode] = command.consume() ;
			return COMMAND_STATE::PARSED;
		}
		if (cmd == "unbind") {
			if (command.args() < 1) return COMMAND_STATE::INVALID;
			auto kcode = get_key_code(command);
			if (command.args() != 0) {
				return COMMAND_STATE::INVALID; 
			}
			kmap[kcode] = "nil";
			return COMMAND_STATE::PARSED;
		}

		if (cmd == "line") {
			if (command.args() != 4) return COMMAND_STATE::INVALID; 
			auto x1 = skc::util::to_number<float>(command.consume());
			auto y1 = skc::util::to_number<float>(command.consume());
			auto x2 = skc::util::to_number<float>(command.consume());
			auto y2 = skc::util::to_number<float>(command.consume());
			window.draw_line(x1, y1, x2, y2);
			return COMMAND_STATE::PARSED;
		}

		if (cmd == "tile") {
			return skc::commands::tile_command(window, command, tmap);
		}
		if (cmd == "draw") {
			auto sub_command = command.consume();
			if (sub_command == "rect") {
				size_t param_count = command.args();
				if (param_count == 4) {
					auto x = skc::util::to_number<double>(command.consume());
					auto y = skc::util::to_number<double>(command.consume());
					auto w = skc::util::to_number<double>(command.consume());
					auto h = skc::util::to_number<double>(command.consume());
					window.fill_rectangle(x, y, w, h);
					return COMMAND_STATE::PARSED;
				}
				return COMMAND_STATE::INVALID;
			}
			if (sub_command == "texture") {
				return render_texture(window, command, tmap);
			}
			return COMMAND_STATE::INVALID;
			
			//return skc::commands::draw_command(window, command,tmap);
		}

		if (cmd == "alpha") {
			if (command.args() != 2) return COMMAND_STATE::INVALID; 
			auto tid = skc::util::to_number<size_t>(command.consume());
			auto alpha = skc::util::to_number<uint8_t>(command.consume());
			window.set_texture_alpha_mod(tid, alpha);
			return COMMAND_STATE::PARSED;

		}
		if (cmd == "seti") {
			auto sub_command = command.consume();
			if (sub_command == "draw_color") {
				return skc::commands::set_draw_color(command, window); 
			}
			if (sub_command == "clear_color") {
				return skc::commands::set_clear_color(command, window); 
			}
			if (sub_command == "render_target") {
				auto r = std::stoi(command.consume());

				window.set_render_target(r);
				return COMMAND_STATE::PARSED;
			}
			return COMMAND_STATE::INVALID; 

		}
	}

}