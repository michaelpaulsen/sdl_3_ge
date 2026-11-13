#pragma once
//stdlib stuff
#include <print>
#include <string> 
#include <array> 
#include <vector>

//3rd party deps
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_stdinc.h>
//first party deps
#include "types.hpp"
#include "../math/Vector.hpp"
#include "./private/should_skip_event.hpp"
#include "key_mod.hpp"
#ifndef ALWAYS_INLINE 
#ifdef _WIN32
#define ALWAYS_INLINE [[msvc::forceinline]]
#elifdef __CLANG__
#define ALWAYS_INLINE [[clang::always_inline]]]
#elseifdef __GNUC__ 
#define ALWAYS_INLINE [[gnu::always_inline]]
#endif
#endif
//TODO(skc) : move to own file?
#ifndef U32
#include <cstdint>
#define U32 static_cast<uint32_t>
#endif // !U32
#ifndef UZ
#define UZ static_cast<size_t>
#endif // !U32
namespace SKC::GE {
	class event_handler {
	private:
		struct keyevent_state_t {
			bool down;
			bool pressed; 
			bool repeat;
			Uint64 last, delta; 
		};
		struct mouse_button_state_t {
			float x, y;
			Uint8 clicks;
			bool down, up,  pressed; 
		};
		template<size_t size>
		using key_state_array_t = std::array<keyevent_state_t, size>;

		bool m_window_resized{ false };
		bool m_quit{ false };
		bool m_system_theme_changed{ false };
		bool m_window_is_minimized{ false };
		bool m_has_key_event{ false };
		bool m_mouse_focus{ false };
		bool m_is_dragging{ false };
		bool m_has_started_dragging{ false };

		bool* m_last_key_state = NULL; 
		types::drop_event_data_type_t m_dropped_data_type{ types::drop_event_data_type_t::NO_DROPPED_DATA };

		key_mod m_keymod_state{};
		types::full_screen_state_change_t m_fullscreen_status{ 0 };
		std::string m_last_dropped_data{};
		std::array<mouse_button_state_t, 256> m_mouse_keys{0};
		//NOTE(skc) m_cursor_position is set by both the mouse and any JOYSTICS 
		//where as m_mouse_position is the raw mouse position.
		SKC::Math::Vect2d m_mouse_position{ 0,0 };
		SKC::Math::Vect2d m_cursor_position{ 0,0 };
		SKC::Math::Vect2d m_last_joy_relitive_pos{ 0,0 };
		SKC::Math::Vect2d m_last_mouse_pos{ 0,0 };
		SKC::Math::Vect2d m_last_mouse_rel_pos{ 0,0 };
		SKC::Math::Vect2d m_drag_vector{ 0,0 };
		SKC::Math::Vect2d m_scroll_wheel_pos{ 0,0 };

		key_state_array_t<256> m_key_states{};
		static const int INVALID_NUMBER_PAD_KEY_CODE = 0xffff; 
		int get_number_pad_idx(int key) {
			//TODO(skc) : distinguish between the KP and main layout numbers. 
			
			switch (key) {
				case SDLK_KP_DIVIDE:   return '/';
				case SDLK_KP_MULTIPLY: return '*';
				case SDLK_KP_MINUS:    return '-';
				case SDLK_KP_PLUS:     return '+'; 
				case SDLK_KP_0:        return '0';
				case SDLK_KP_1:        return '1';
				case SDLK_KP_2:        return '2';
				case SDLK_KP_3:        return '3';
				case SDLK_KP_4:        return '4';
				case SDLK_KP_5:        return '5';
				case SDLK_KP_6:        return '6';
				case SDLK_KP_7:        return '7';
				case SDLK_KP_8:        return '8';
				case SDLK_KP_9:        return '9';
				case SDLK_KP_PERIOD:   return '.';
				default:               return INVALID_NUMBER_PAD_KEY_CODE;

			}
			std::unreachable(); 
		}
		
		bool is_mod_key(int key) {
			return (key == SDLK_LALT || key == SDLK_RALT
				|| key == SDLK_MENU || key == SDLK_CAPSLOCK
				|| key == SDLK_LCTRL || key == SDLK_RCTRL); 
		}
		ALWAYS_INLINE void process_keboard_state(keyevent_state_t& ks, SDL_Scancode scan_code, const bool* key_states) {
			if (not key_states[scan_code]) {
				ks.down = false;
				ks.repeat = false;
				ks.pressed = false;
				ks.last = 0;
				ks.delta = 0;
				return;
			}
			ks.down = true;
			if (not m_last_key_state[scan_code]) {
				ks.last = SDL_GetTicksNS();
				ks.delta = 0;
				ks.pressed = true; 
				ks.repeat = false;
				return;
			}
			ks.pressed = false; 
			ks.repeat = true;
			ks.delta = SDL_GetTicksNS() - ks.last;
		}
		void process_keboard_state(types::special_keys key, SDL_Scancode scan_code, const bool* key_states) {
			process_keboard_state(m_key_states.at(UZ(key)), scan_code, key_states);

		}
	public:
		enum {
			LEFT_MOUSE_BUTTON = 1,
			MIDDLE_MOUSE_BUTTON,
			RIGHT_MOUSE_BUTTON,
		};

		event_handler() {
			int count;
			SDL_GetKeyboardState(&count);
			m_last_key_state = new bool[count]; 
		};
		~event_handler() {
			delete[] m_last_key_state;
		}
		//NOTE(skc): 
		//want to not be able to move or copy this as it doesn't make sense... 
		//if you can make sense of what it would mean to copy and / or move a event handler 
		//feel free to implement these your self.. (I may or may not accept your PR) 
		event_handler(event_handler&) = delete;
		event_handler(event_handler&&) = delete;
		event_handler operator=(event_handler&&) = delete;

		//NOTE(SKC): instead of making getter and setter for this I am just goint to make it public
		bool arrow_keys_alias_WASD = false;
		//== The Getters for the State. 
		//maybe shouldn't be const... (so that I can reset the flag when called)
		bool theme_changed()        const noexcept { return m_system_theme_changed; }
		bool is_minimized()         const noexcept { return m_window_is_minimized; }
		bool quit()                 const noexcept { return m_quit; }
		bool has_dropped_data()     const noexcept { return m_dropped_data_type != types::drop_event_data_type_t::NO_DROPPED_DATA; }
		bool has_key_event()        const noexcept { return m_has_key_event; }
		bool window_resized()       const noexcept { return m_window_resized; }
		bool mouse_focus()          const noexcept { return m_mouse_focus; }
		bool is_dragging()          const noexcept { return m_is_dragging; }
		bool has_started_dragging() const noexcept { return m_has_started_dragging; }

		//NOTE(skc) : 
		// this is not const because it is kinda important
		// to not ignore the user when they drop something into the
		// aplication so the flag is only cleared when the consumer
		// calls this function.	
		auto dropped_data_type() {
			auto ret = m_dropped_data_type;
			m_dropped_data_type = types::drop_event_data_type_t::NO_DROPPED_DATA;
			return ret ; 
		}
		auto get_mouse_button_state(unsigned char button) const {
			return m_mouse_keys.at(UZ(button));
		}
		auto get_key_mods() const noexcept { return m_keymod_state; }
		auto entered_full_screen() const noexcept { return m_fullscreen_status; }
		auto last_joy_pos_r() const noexcept { return m_last_joy_relitive_pos; }
		auto mouse_position() const noexcept { return m_mouse_position; }
		auto relative_mouse_position() const noexcept { return m_last_mouse_rel_pos; }
		auto cursor_position() const noexcept { return m_cursor_position; }
		auto drag_vector() const noexcept { return m_drag_vector; }
		auto scroll_wheel_pos() const noexcept { return m_scroll_wheel_pos; }
		auto get_key_state(unsigned char key) const noexcept { return m_key_states.at(UZ(tolower(int(key)))); }
		auto get_key_state(types::special_keys key) const noexcept { return m_key_states.at(UZ(tolower(int(key)))); }
		
		void pollevents() noexcept {
			int count;
			SDL_GetKeyboardState(&count);
			memcpy(m_last_key_state, SDL_GetKeyboardState(NULL), count); 

			m_has_key_event = false;
			m_window_resized = false;
			m_system_theme_changed = false;
			m_fullscreen_status = types::full_screen_state_change_t::NO_CHANGE;
			m_scroll_wheel_pos = { 0,0 };
			m_last_mouse_rel_pos = { 0,0 };
			SDL_Event evnt{};
			//NOTE(skc): reset the pressed state for all of the mouse keys. 
			for (auto& key : m_mouse_keys) {
				key.up = false; 
				if (key.down && key.pressed) {
					key.pressed = false;
				}
			}

			while (SDL_PollEvent(&evnt)) {
				auto type = evnt.type;
				//NOTE(skc): if the event is one that we don't whant to process then there's no reason to process it.
				if (PRIVATE::should_sikp_event(type)) continue;
				switch (evnt.type) {
#pragma region ==MISC EVENTS==
					//NOTE(SKC): this kinda assumes that the app only has one window.
					//Maybe we should not assume that?
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
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
					m_dropped_data_type = types::drop_event_data_type_t::FILE_DATA;
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
					m_fullscreen_status = types::full_screen_state_change_t::ENTERED_FULL_SCREEN;
					break;

				}
				case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN: {
					m_fullscreen_status = types::full_screen_state_change_t::EXITED_FULLSCREEN;
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
					if (m_has_started_dragging) {
						m_is_dragging = true;
					}
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
					auto& mb_state = m_mouse_keys.at(UZ(button.button));
					mb_state.x = button.x;
					mb_state.y = button.y;
					if (!mb_state.down) {
							mb_state.down = true;
							mb_state.pressed = true;
					} 
					if (button.button == 1) {
						m_has_started_dragging = true;
						m_last_mouse_pos = { static_cast<double>(button.x), static_cast<double>(button.y) };
					}
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_UP: {
					auto button = evnt.button;
					auto& ms = m_mouse_keys.at(UZ(button.button)); 
					ms.clicks = 0;
					ms.down = false;
					ms.up = true;
					ms.x = 0; 
					ms.y = 0; 
					m_is_dragging = false;
					m_has_started_dragging = false;
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
				case SDL_EVENT_KEY_UP:
				{		
					auto e = evnt.key; 
					m_keymod_state = key_mod(e.mod); 
					break;
				}

#pragma endregion
#pragma region === AUDIO DEVICE EVENTS 
// TODO(skc) : implement audio device events 
				case SDL_EVENT_AUDIO_DEVICE_ADDED:
				case SDL_EVENT_AUDIO_DEVICE_REMOVED:
				case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED: {
					std::println("audio device events are not implemented");
					break;
				}
#pragma endregion 
				default: {
					std::println(" unhandled event type {:#06x}", type);
					break;
				}
				}
			}
			//Note(skc) :  this is a view into the keyboard state! 
			// It is not copied it doesn't need to be freed!
			auto key_states = SDL_GetKeyboardState(NULL); 
			for(int key = SDLK_SPACE; key <= SDLK_TILDE; ++key){
				//todo make this take modifier keys into account; 
				auto& ks = m_key_states.at(key);
				auto scan_code = SDL_GetScancodeFromKey(key, NULL);
				process_keboard_state(ks, scan_code, key_states);

			}
			for (int x = SDLK_KP_DIVIDE; x <= SDLK_KP_PERIOD; ++x) {
				auto scan_code = SDL_GetScancodeFromKey(x, NULL);
				if (auto key = get_number_pad_idx(x);
					key != INVALID_NUMBER_PAD_KEY_CODE) {
					auto& ks = m_key_states.at(key);
					process_keboard_state(ks, scan_code, key_states);
				}
			}
			//NOTE(skc): 
			// Since SDLK_SPACE is the first key code that we 
			// actually iterate through we can use the first 0x1f (31) 
			// positions in the array for what ever scan codes that we want.  
			process_keboard_state(types::special_keys::BACKSPACE, SDL_SCANCODE_BACKSPACE, key_states);
			process_keboard_state(types::special_keys::RETURN,    SDL_SCANCODE_RETURN,    key_states);
			process_keboard_state(types::special_keys::INSERT,    SDL_SCANCODE_INSERT,    key_states);
			process_keboard_state(types::special_keys::ESC,       SDL_SCANCODE_ESCAPE,    key_states);
			process_keboard_state(types::special_keys::DEL,       SDL_SCANCODE_DELETE,    key_states);
			process_keboard_state(types::special_keys::HOME,      SDL_SCANCODE_HOME,      key_states);
			process_keboard_state(types::special_keys::END,       SDL_SCANCODE_END,       key_states);
			process_keboard_state(types::special_keys::PAGEUP,    SDL_SCANCODE_PAGEUP,    key_states);
			process_keboard_state(types::special_keys::PAGEDOWN,  SDL_SCANCODE_PAGEDOWN,  key_states);
			process_keboard_state(types::special_keys::UP,        SDL_SCANCODE_UP,        key_states);
			process_keboard_state(types::special_keys::DOWN,      SDL_SCANCODE_DOWN,      key_states);
			process_keboard_state(types::special_keys::LEFT,      SDL_SCANCODE_LEFT,      key_states);
			process_keboard_state(types::special_keys::RIGHT,     SDL_SCANCODE_RIGHT,     key_states);
			process_keboard_state(types::special_keys::F1,        SDL_SCANCODE_F1,        key_states);
			process_keboard_state(types::special_keys::F2,        SDL_SCANCODE_F2,        key_states);
			process_keboard_state(types::special_keys::F3,        SDL_SCANCODE_F3,        key_states);
			process_keboard_state(types::special_keys::F4,        SDL_SCANCODE_F4,        key_states);
			process_keboard_state(types::special_keys::F5,        SDL_SCANCODE_F5,        key_states);
			process_keboard_state(types::special_keys::F6,        SDL_SCANCODE_F6,        key_states);
			process_keboard_state(types::special_keys::F7,        SDL_SCANCODE_F7,        key_states);
			process_keboard_state(types::special_keys::F8,        SDL_SCANCODE_F8,        key_states);
			process_keboard_state(types::special_keys::F9,        SDL_SCANCODE_F9,        key_states);
			process_keboard_state(types::special_keys::F10,       SDL_SCANCODE_F10,       key_states);
			process_keboard_state(types::special_keys::F11,       SDL_SCANCODE_F11,       key_states);
			process_keboard_state(types::special_keys::F12,       SDL_SCANCODE_F12,       key_states);
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