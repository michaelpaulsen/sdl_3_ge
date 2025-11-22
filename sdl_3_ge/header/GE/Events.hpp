#pragma once
//stdlib stuff
#include <print>
#include <functional>
#include <vector>
#include <algorithm>
#include <expected> 
#include <string> 
#include <array> 
#include <filesystem> 
#include <cassert>
#include <stacktrace>

//3rd party deps
#include <SDL3/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

//first party deps
#include "../math/Vector.hpp"

//TODO(skc) : move to own file?
#ifndef U32
#define U32 static_cast<uint32_t>
#endif // !U32
#ifndef UZ
#define UZ static_cast<size_t>
#endif // !U32
//TODO(skc) : move this to its own file or something. 
#define CATCH_FINAL catch (...) {\
auto stack_trace = std::stacktrace::current(0);\
std::print("error {}\nat\n\t{}", "YOU BROKE ME! I HAVE NO IDEA HOW YOU DID, BUT I AM BROKEN NOW!", stack_trace);\
std::exit(-1); \
}

namespace SKC::GE {
	//TODO(skc) : this file is kinda getting long.
	//move to own file?
	//TODO(skc) add converstion operator for uint16_t
	struct key_mod {
		bool lshift,
			rshift,
			rctrl,
			lctrl,
			lalt,
			ralt,
			lgui,
			rgui,
			num,
			caps,
			scroll;
		explicit key_mod() :
			lshift(false),
			rshift(false),
			rctrl(false),
			lctrl(false),
			ralt(false),
			lalt(false),
			rgui(false),
			lgui(false),
			num(false),
			caps(false),
			scroll(false)
		{
		}
		key_mod(const key_mod& other) noexcept {
			lshift = other.lshift;
			rshift = other.rshift;
			rctrl = other.rctrl;
			lctrl = other.lctrl;
			ralt = other.ralt;
			lalt = other.lalt;
			rgui = other.rgui;
			lgui = other.lgui;
			num = other.num;
			caps = other.caps;
			scroll = other.scroll;
		}
		key_mod(key_mod&& other) noexcept {
			lshift = other.lshift;
			rshift = other.rshift;
			rctrl = other.rctrl;
			lctrl = other.lctrl;
			ralt = other.ralt;
			lalt = other.lalt;
			rgui = other.rgui;
			lgui = other.lgui;
			num = other.num;
			caps = other.caps;
			scroll = other.scroll;
			other = key_mod(0);
		}
	
		void operator=(const key_mod& other) noexcept {
			lshift = other.lshift;
			rshift = other.rshift;
			rctrl = other.rctrl;
			lctrl = other.lctrl;
			ralt = other.ralt;
			lalt = other.lalt;
			rgui = other.rgui;
			lgui = other.lgui;
			num = other.num;
			caps = other.caps;
			scroll = other.scroll;
		}
		explicit key_mod(uint16_t keymod) :
			lshift(keymod& SDL_KMOD_LSHIFT),
			rshift(keymod& SDL_KMOD_RSHIFT),
			rctrl(keymod& SDL_KMOD_RCTRL),
			lctrl(keymod& SDL_KMOD_LCTRL),
			ralt(keymod& SDL_KMOD_RALT),
			lalt(keymod& SDL_KMOD_LALT),
			rgui(keymod& SDL_KMOD_RGUI),
			lgui(keymod& SDL_KMOD_LGUI),
			num(keymod& SDL_KMOD_NUM),
			caps(keymod& SDL_KMOD_CAPS),
			scroll(keymod& SDL_KMOD_SCROLL)
		{
		}

		~key_mod() = default;

		bool shift() const noexcept {
			return rshift || lshift;
		}
		bool alt() const noexcept {
			return ralt || lalt;
		}
		bool gui() const noexcept {
			return rgui || lgui;

		}
		bool ctrl() const noexcept {
			return rctrl || lctrl;

		}
		//TODO(skc) : implement -> SDL_KMOD_LEVEL5



	};

	enum struct full_screen_state_change_t : unsigned char {
			NO_CHANGE,
			EXITED_FULLSCREEN,
			ENTERED_FULL_SCREEN

		};
	enum struct drop_event_data_type_t: unsigned char {
			NO_DROPPED_DATA,
			FILE_DATA,
			TEXT_DATA

		};
	enum struct arrow_direction_t : unsigned char {
		LEFT,
		UP,
		RIGHT,
		DOWN,
		MAX
	};
	
	
	template<bool _use_IMGUI = false>
	class event_handler {
	private:
		struct keyevent_state_t {
			bool down;              
			bool repeat;
		};
		struct mouse_button_state_t {
			Uint8 clicks;
			bool down;
			float x, y;
		};
		template<typename state_t> 
		using state_array_t = std::array<state_t, 256>;
		template<size_t size>
		using key_state_array_t = std::array<keyevent_state_t, size>;

		char m_last_key =0; 
		bool m_window_resized{ false }; 
		bool m_quit{ false }; 
		bool m_system_theme_changed{ false }; 
		bool m_window_is_minimized{ false }; 
		bool m_has_key_event{ false }; 
		bool m_mouse_focus{ false }; 
		bool m_is_dragging{ false };
		bool m_has_started_dragging{ false };

		drop_event_data_type_t m_dropped_data_type{ drop_event_data_type_t::NO_DROPPED_DATA };

		key_mod m_keymod_state{}; 
		full_screen_state_change_t m_fullscreen_status{ 0 };
		std::string m_last_dropped_data{}; 
		std::array<mouse_button_state_t, 256> m_mouse_keys;

		//NOTE(skc) m_cursor_position is set by both the mouse and any JOYSTICS 
		//where as m_mouse_position is the raw mouse position.
		SKC::Math::Vect2d m_mouse_position{ 0,0 };
		SKC::Math::Vect2d m_cursor_position{ 0,0 };
		SKC::Math::Vect2d m_last_joy_relitive_pos{ 0,0 };
		SKC::Math::Vect2d m_last_mouse_pos{ 0,0 };
		SKC::Math::Vect2d m_last_mouse_rel_pos{ 0,0 };
		SKC::Math::Vect2d m_drag_vector{ 0,0 };		
		SKC::Math::Vect2d m_scroll_wheel_pos{ 0,0 };
		state_array_t<keyevent_state_t> m_key_states{};

		key_state_array_t<UZ(arrow_direction_t::MAX)> m_arrow_state{};
		
	public : 
		enum {
			RIGHT_MOUSE_BUTTON = 1,
			LEFT_MOUSE_BUTTON,
			MIDDLE_MOUSE_BUTTON
		};
		
		event_handler() = default; 
		~event_handler() = default;

		//want to not be able to move or copy this as it doesn't make sense... 
		//if you can make sense of what it would mean to copy and / or move a event handler 
		//feel free to implement these your self.. (I may or may not accept your PR) 
		event_handler(event_handler&) = delete;
		event_handler(event_handler&&) = delete;
		event_handler operator=(event_handler&&) = delete;

		//instead of making getter and setter for this I am just goint to make it public
		bool arrow_keys_alias_WASD = false;
		//bool m_up, m_down, m_left, m_right;

		//== The Getters for the State. 
		//maybe shouldn't be const... (so that I can reset the flag when called)
		bool theme_changed() const noexcept { return m_system_theme_changed; }
		bool is_minimized() const noexcept { return m_window_is_minimized; }
		bool quit() const noexcept { return m_quit; }
		bool has_dropped_data() const noexcept { return m_dropped_data_type != drop_event_data_type_t::NO_DROPPED_DATA; }
		bool has_key_event() const noexcept { return m_has_key_event; }
		bool window_resized() const noexcept { return m_window_resized; }
		bool mouse_focus() const noexcept { return m_mouse_focus; }
		bool is_dragging() const noexcept { return m_is_dragging; }
		bool has_started_dragging() const noexcept { return m_has_started_dragging; }

		//NOTE(skc) : this is not const because it is kinda important to not ignore the user when they
		// drop something into the aplication so the flag is only cleared when the consumer calls this function.	
		//NOTE(skc) :IF YOU'RE TRYING TO ACCEPT DATA FROM "DROP" EVENTS CALL THIS EVERY FRAME DO NOT
		//MAKE YOUR USERS WAIT FOR THEIR INPUT TO BE PROCESSED! 
		auto dropped_data_type() { 
			auto ret = m_dropped_data_type;
			m_dropped_data_type = drop_event_data_type_t::NO_DROPPED_DATA; 
			return ret ; 
		}
		auto get_mouse_button_state(unsigned char button) const {
			return m_mouse_keys.at(UZ(button));
		}
		auto get_arrow_key_state(arrow_direction_t arrow) const {
			if (arrow == arrow_direction_t::MAX) return keyevent_state_t{ false,false };
			return m_arrow_state.at(UZ(arrow));
		}
		auto& get_key_mods() const noexcept { return m_keymod_state; }
		auto entered_full_screen() const noexcept { return m_fullscreen_status; }
		auto last_joy_pos_r() const noexcept { return m_last_joy_relitive_pos; }
		auto mouse_position() const noexcept { return m_mouse_position; }
		auto relative_mouse_position() const noexcept { return m_last_mouse_rel_pos; }
		auto cursor_position() const noexcept { return m_cursor_position; }
		auto drag_vector() const noexcept { return m_drag_vector; }
		auto scroll_wheel_pos() const noexcept { return m_scroll_wheel_pos; }
		auto get_key_state(unsigned char key) const noexcept  {
			try {
				return m_key_states.at(UZ(key));
			}
			CATCH_FINAL
			}

		char get_last_key() const noexcept {
			return m_last_key; 
		}
		void pollevents() noexcept {
			m_has_started_dragging = false;
			m_has_key_event = false; 
			m_window_resized = false;
			m_system_theme_changed = false;
			m_fullscreen_status = full_screen_state_change_t::NO_CHANGE;
			m_scroll_wheel_pos = { 0,0 };
			m_last_mouse_rel_pos = { 0,0 }; 
			SDL_Event evnt{}; 
			while (SDL_PollEvent(&evnt)) {
				if constexpr (_use_IMGUI) {
					ImGui_ImplSDL3_ProcessEvent(&evnt);
				}
				auto type = evnt.type; 
				switch (evnt.type) {
#pragma region ==MISC EVENTS==
				case SDL_EVENT_QUIT: 
				case SDL_EVENT_RENDER_DEVICE_LOST: {
					m_quit = true; 
					break; 
				}
				case SDL_EVENT_SYSTEM_THEME_CHANGED: {
					m_system_theme_changed = true; 
					break; 
				}
#pragma endregion
#pragma region ==SDL_WINDOW_EVENTS==
												 
				case SDL_EVENT_WINDOW_FOCUS_LOST: {
					break; 
				}
				case SDL_EVENT_WINDOW_MINIMIZED:
				case SDL_EVENT_WINDOW_HIDDEN: {
					m_window_is_minimized = true;
					break; 
				}
				case SDL_EVENT_DROP_FILE: {
					m_dropped_data_type = drop_event_data_type_t::FILE_DATA; 
					m_last_dropped_data = evnt.drop.data; 
					std::print("file dropped with data {}", evnt.drop.data);
					break; 
				}
				case SDL_EVENT_WINDOW_MAXIMIZED:
				case SDL_EVENT_WINDOW_RESTORED:
				case SDL_EVENT_WINDOW_SHOWN: {
					m_window_is_minimized = false;
					m_window_resized = true;
					break; 
				}
				case SDL_EVENT_WINDOW_ENTER_FULLSCREEN: {
					m_fullscreen_status = full_screen_state_change_t::ENTERED_FULL_SCREEN;
					break; 

				}
				case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: {
					m_fullscreen_status = full_screen_state_change_t::EXITED_FULLSCREEN;
					break;

				}
				case SDL_EVENT_WINDOW_RESIZED: {
					m_window_resized = true; 
					break; 
				}
#pragma endregion
#pragma region ==MOUSE EVENTS ==
//===MOUSE EVENTS ===//
				case SDL_EVENT_MOUSE_MOTION: {
					auto x = evnt.motion.x, y = evnt.motion.y; 
					auto xrel = evnt.motion.xrel, yrel = evnt.motion.yrel;
					m_mouse_position = { x, y };
					m_cursor_position = { x, y };
					m_last_mouse_rel_pos = { xrel,yrel };
					break; 
				}
				case SDL_EVENT_MOUSE_BUTTON_DOWN: 
				{
					auto button = evnt.button;
					mouse_button_state_t state = { button.clicks, button.down, button.x, button.y };
					m_mouse_keys.at(UZ(button.button)) = state;
					if (button.button == 1) {
						m_has_started_dragging = true;
						m_is_dragging = true;
						m_last_mouse_pos = { static_cast<double>(button.x), static_cast<double>(button.y) };
					}
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_UP: {
					auto button = evnt.button;
					mouse_button_state_t state = { button.clicks, button.down, static_cast<float>(button.x), static_cast<float>(button.y) };
					m_mouse_keys.at(UZ(button.button)) = state;
					m_is_dragging = false;
					break;
				}
				case SDL_EVENT_MOUSE_WHEEL: {
					m_scroll_wheel_pos = { evnt.wheel.x,  evnt.wheel.y };
					break;
				}
				case SDL_EVENT_WINDOW_MOUSE_ENTER: {
					m_mouse_focus = true;
					break;
				}
				case SDL_EVENT_WINDOW_MOUSE_LEAVE:
				{
					m_mouse_focus = false;
					break; 
				}
#pragma endregion
#pragma region ===KEYBOARD EVENTS===
				//===KEYBOARD EVENTS===//
				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP : {
					m_has_key_event = true; 
					auto keyevnt = evnt.key; 
					auto key = keyevnt.key;
					auto down = keyevnt.down;
					auto repeat = keyevnt.repeat;
					m_keymod_state = key_mod(keyevnt.mod);
					if (key == SDLK_EQUALS) {
						m_key_states.at(U32('+')) = { down,repeat };
						m_key_states.at(U32('=')) = { down,repeat };
						break; 
					}
					if (key == SDLK_ASTERISK || key == SDLK_KP_MULTIPLY) {
						m_key_states.at(U32('*')) = { down, repeat };
						break;
					}
					
					
					if (key < 256) {
						m_key_states.at(key) = { down,repeat };
						if (evnt.type == SDL_EVENT_KEY_DOWN && !repeat) {
							m_last_key = (char)key; 
						}
						else {
							m_last_key = 0; 
						}
						break; 
					}

					//NOTE(skc) : I set the m_arrow_state even if arrow_keys_alias_WASD is true because 
					//the consumer may still have contextual reasons to treat the arrow keys differently 
					//while still wanting them to alias their respective keys. 
					if (key == SDLK_LEFT) {
						if (arrow_keys_alias_WASD) m_key_states.at(U32('a')) = { keyevnt.down,keyevnt.repeat }; 
						m_arrow_state.at(UZ(arrow_direction_t::LEFT)) = { down, repeat };
						break; 
					}
					if (key == SDLK_RIGHT) {
						if (arrow_keys_alias_WASD) m_key_states.at(U32('d')) = { down,repeat };
						m_arrow_state.at(UZ(arrow_direction_t::RIGHT)) = { down, repeat };
						break;
					}
					if (key == SDLK_UP) {
						if (arrow_keys_alias_WASD) m_key_states.at(U32('w')) = { down,repeat };
						m_arrow_state.at(UZ(arrow_direction_t::UP)) = { down, repeat };
						break;
					}
					if (key == SDLK_DOWN) {
						if (arrow_keys_alias_WASD) m_key_states.at(U32('s')) = { down,repeat };
						m_arrow_state.at(UZ(arrow_direction_t::DOWN)) = { down, repeat };
						break;
					}
					
					//NOTE(skc) : for some reason the char codes for the KP are 
					//1234567890 not the ASCII layout of 0123456789 ... 
					//this takes the key code and sets the respective bit

					//TODO(skc) : distinguish between the KP and main layout numbers. 
					if (key == SDLK_KP_0) {
						m_key_states.at(U32('0')) = { down,repeat };
						break;
					}
					if (key >= SDLK_KP_1 && key < SDLK_KP_0) {
						key = '1' + ( key - SDLK_KP_1);
						m_key_states.at(key) = { down,repeat };
						break; 
					}
					if(key == SDLK_LALT || key == SDLK_RALT) {
						break; 
					}
					if (key == SDLK_LCTRL || key == SDLK_RCTRL) {
						break;
					}
					if (key == SDLK_MENU || key == SDLK_CAPSLOCK) {
						break;
					}
					
					std::print("UNKNOWN EXSTENDED KEY CODE 0x{:>0x}\r", key);
					break;

				}
#pragma endregion
#pragma region ===TEXT INPUT EVENTS===	
				//===TEXT INPUT EVENTS===	
				case SDL_EVENT_TEXT_EDITING:
				case SDL_EVENT_TEXT_INPUT:               /**< Keyboard text input */
				case SDL_EVENT_KEYMAP_CHANGED: {
					std::print("KB event not handled 0x{:>04x}\r", type);
					break;
				}
#pragma endregion
				default: {
					std::print(" unhandled event type 0x{:>04X}\r", type);
					break;
				}
				}
			}
			if (m_is_dragging) {
				m_drag_vector = m_mouse_position - m_last_mouse_pos;
				m_last_mouse_pos = m_mouse_position; 
				}
			else {
				m_drag_vector = { 0,0 };
			}
		}
	}; 
}


//TODO(SKC): move to own file.
//TODO(SKC) : make this so that it only outputs in dbg mode... 

//NOTE(SKC): FORMAT_STRING_GET is assumed to be undefed at the end of this function  
// things may break if you remove the #undef FORMAT_STRING_GET. 
#define FORMAT_STRING_GET(prop, val) string += val; \
	if (obj.prop) {string += " true ";} \
	else {string += " false";} \
	if(new_line)string += '\n';\
	else string += ' ';
template <>
struct std::formatter<SKC::GE::key_mod> {
	bool new_line = false;
	
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		while (pos != ctx.end() && *pos != '}') {
			if (*pos == 'n')
				new_line = true;
			++pos;
		}
		return pos;
		//FIXME(skc): this should throw if *pos != '}' 
		//because this should error when it is not. 
		//however this is not an huge issue...

	}
		auto format(const SKC::GE::key_mod & obj, std::format_context & ctx) const {
			auto string = std::string();
			if (new_line) {
				string += "===KEY MODIFIER STATE===\n";
			}
			else {
			string += "Key State ->";

			}
			
			FORMAT_STRING_GET(lshift, "LSFT ");
			FORMAT_STRING_GET(rshift, "RSFT ");
			FORMAT_STRING_GET(lctrl, "RCTRL ");
			FORMAT_STRING_GET(rctrl, "LCTRL ");
			FORMAT_STRING_GET(lalt, "LALT ");
			FORMAT_STRING_GET(ralt, "LALT ");
			FORMAT_STRING_GET(lgui, "LMEN ");
			FORMAT_STRING_GET(rgui, "RMEN ");
			FORMAT_STRING_GET(num, "NUMLOK ");
			FORMAT_STRING_GET(caps, "CAPSLOK ");
			FORMAT_STRING_GET(scroll, "SCRLLOK ");
			return std::format_to(ctx.out(),"{}",  string);
		}
	
};
#undef FORMAT_STRING_GET