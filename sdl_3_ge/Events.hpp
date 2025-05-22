#pragma once

#include <SDL3/SDL.h>

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
#include "./Vector.hpp"
#ifndef U32
#define U32 static_cast<uint32_t>
#endif // !U32

namespace SKC::GE {
	
	class event_handler {
	public:
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
	private :
		struct keyevent_state_t {
			bool down;              
			bool repeat;
		};
		template<typename state_t> 
		using state_array_t = std::array<state_t, 256>;
		bool m_window_resized{ false }; 
		bool m_quit{ false }; 
		bool m_system_theme_changed{ false }; 
		bool m_window_is_minimized{ false }; 
		
		drop_event_data_type_t m_dropped_data_type{ drop_event_data_type_t::NO_DROPPED_DATA};


		full_screen_state_change_t m_fullscreen_status{ 0 };
		std::string m_last_dropped_data{}; 

		//NOTE(skc) m_cursor_position is set by both the mouse and any JOYSTICS 
		//where as m_mouse_position is the raw mouse position.
		//the way that this is updated makes it theroetically possible to miss imputs when the same input is 
		//inputed more than once a frame but even at 30 fps that is nearly imposible
		//+ I think that every GE in existance has a simular system... 
		bool m_up, m_down, m_left, m_right;
		SKC::Math::Vect2d m_mouse_position{ 0,0 };
		SKC::Math::Vect2d m_cursor_position{ 0,0 };
		SKC::Math::Vect2d m_last_joy_relitive_pos{ 0,0 };
		
		state_array_t<keyevent_state_t> m_key_states{};
		state_array_t<bool> m_mouse_button_states{};
		//const uint32_t LARROW = ;
	public : 
		
		
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
		bool theme_changed() const { return m_system_theme_changed;}
		bool is_minimized() const { return m_window_is_minimized;}
		bool quit() const { return m_quit;  }
		bool has_dropped_data() const { 
			return m_dropped_data_type != drop_event_data_type_t::NO_DROPPED_DATA; 
		}

		auto    up() const { return m_up; }
		auto  down() const { return m_down; }
		auto  left() const { return m_left; }
		auto right() const { return m_left; }
		//NOTE(skc) : this is not const because it is kinda important to not ignore the user when they
		// drop something into the aplication so the flag is only cleared when the consumer calls this function.	
		//NOTE(skc) :IF YOU'RE TRYING TO ACCEPT DATA FROM "DROP" EVENTS CALL THIS EVERY FRAME DO NOT
		//MAKE YOUR USERS WAIT FOR THEIR INPUT TO BE PROCESSED! 
		auto dropped_data_type() { 
			auto ret = m_dropped_data_type;
			m_dropped_data_type = drop_event_data_type_t::NO_DROPPED_DATA; 
			return ret ; 
		}
		 

		auto entered_full_screen() const { return m_fullscreen_status;  }
		auto last_joy_pos_r() const { return m_last_joy_relitive_pos; }
		auto mouse_position() const { return m_mouse_position; }
		auto cursor_position() const { return m_cursor_position; }
		auto get_key_state(unsigned char key) {
			try {
				return m_key_states.at(static_cast<size_t>(key));
			}
			catch (...) {
				//TODO(skc) : make a macro outa this... 
				auto stack_trace = std::stacktrace::current(0);
				std::print("error {}\nat\n\t{}", "YOU BROKE ME! I HAVE NO IDEA HOW YOU DID, BUT I AM BROKEN NOW!", stack_trace);
				
				//this should crash because it should be litterally mathmatically imposible to reach this... 
				assert(false);
			}
		}

		void pollevents() {
			m_window_resized = false;
			m_system_theme_changed = false;
			m_fullscreen_status = full_screen_state_change_t::NO_CHANGE;
			SDL_Event evnt{}; 
			while (SDL_PollEvent(&evnt)) {
				auto type = evnt.type; 
				switch (evnt.type) {
				case SDL_EVENT_QUIT: 
				case SDL_EVENT_RENDER_DEVICE_LOST: {
					m_quit = true; 
					break; 
				}
				case SDL_EVENT_SYSTEM_THEME_CHANGED: {
					m_system_theme_changed = true; 
					break; 
				}
				//MOUSE EVENTS
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
				case SDL_EVENT_WINDOW_RESTORED: {
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
//===MOUSE EVENTS ===//
				case SDL_EVENT_MOUSE_MOTION: {
					auto x = evnt.motion.x, y = evnt.motion.y; 
					m_mouse_position = { x, y};
					m_cursor_position = { x, y };
					break; 
				}
				
				
				case SDL_EVENT_MOUSE_BUTTON_DOWN: 
				case SDL_EVENT_MOUSE_BUTTON_UP: {
					
					break; 
				}
//===EYBOARD EVENTS===//
				case SDL_EVENT_KEY_DOWN:
				case SDL_EVENT_KEY_UP : {
					auto keyevnt = evnt.key; 
					auto key = keyevnt.key;
					//
					if (key < 256) {
						m_key_states.at(key) = { keyevnt.down,keyevnt.repeat };
						break; 
					}
					switch (key) {
						case SDLK_LEFT: {
							if (arrow_keys_alias_WASD) m_key_states.at(U32('a')) = { keyevnt.down,keyevnt.repeat }; 
							break; 
						}
						case SDLK_RIGHT: {
							if (arrow_keys_alias_WASD) m_key_states.at(U32('d')) = { keyevnt.down,keyevnt.repeat };

							break;
						}
						case SDLK_UP: {
							if (arrow_keys_alias_WASD) m_key_states.at(U32('w')) = { keyevnt.down,keyevnt.repeat };

							break;
						}
						case SDLK_DOWN: {
							if (arrow_keys_alias_WASD) m_key_states.at(U32('s')) = { keyevnt.down,keyevnt.repeat };
							break;
						}
						default: {
							if (key >= SDLK_KP_1 && key <= SDLK_KP_0) {
								key -= (SDLK_KP_1);
								key += '1'; 
								if (key == SDLK_KP_0) key = '0';
								
								m_key_states.at(key) = { keyevnt.down,keyevnt.repeat };
								break; 
							}
							std::print("UNKNOWN EXSTENDED KEY CODE 0x{:>0x}\r", key);
							break;

						}
					}
					break; 
				}
				case SDL_EVENT_TEXT_EDITING:
				case SDL_EVENT_TEXT_INPUT:               /**< Keyboard text input */
				case SDL_EVENT_KEYMAP_CHANGED: {
					std::print("KB event not handled {:#>04x}\r", type);
					break;
				}
				default: 
					std::print(" unhandled event type {:#>04x}\r", type);
				}
			}
		}
	}; 
}