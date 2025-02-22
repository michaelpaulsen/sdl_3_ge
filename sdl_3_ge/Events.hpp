#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <functional>
#include <algorithm>
#include <expected> 
#include <string> 
#include <unordered_map> 

namespace SKC::GE {
	enum class event_t{
		CONTINUE,
		//do not stop processing events 
		BREAK, 
		//stop processing events for this frame and type but not quit the application  
		QUIT, 
		//quit the application...
		JOY_SITCK_EVENT, 
		// the event was a event for the older [and lower level] joystick api 
		//should be treated by the game the same as CONTINUE unless they're using the
		//joystick api in which case they need to provide their own event handleing 
		NO_FUNCT,
		//no call back provided
		//in release mode should be treated as CONTINUE
		//in debug should print that the callback is not defined... 
		

	};
	using event_funct_t = std::function < event_t(SDL_Event)>;

	struct event_discriptor {
		SDL_EventType id;
		event_funct_t funct;
		bool operator==(SDL_EventType oid) const {
			return id == oid;
		}
		bool operator==(int oid) const {
			return id == oid;
		}


		//if you're calling the below then you're 
		//doing something funky but still feel like 
		//this should be here. 
		bool operator==(event_discriptor other) const {
			return other.id == id;
		}

	};
	auto default_event_func = [](SDL_Event e) {
		return event_t::NO_FUNCT;
	};
	
	class event_handler {
		
		using event_list_t = std::vector<event_discriptor>;
		using setter_t = std::expected<std::string, std::string>; 
		using et = SDL_EventType; 
			event_list_t m_events = {};
		std::unordered_map<Uint32, SDL_Gamepad*> m_game_pads = {};
		bool is_joy_event(et id) {
			return (id > SDL_EVENT_JOYSTICK_AXIS_MOTION && id < SDL_EVENT_JOYSTICK_UPDATE_COMPLETE); 
		}
		bool is_member_function(et id) {
			bool a = (id == SDL_EVENT_GAMEPAD_ADDED);
			a |= (id == SDL_EVENT_GAMEPAD_REMOVED);
			a |= (id == SDL_EVENT_GAMEPAD_BUTTON_DOWN);
			a |= (id == SDL_EVENT_GAMEPAD_BUTTON_UP);
			return a; 
		}
		bool is_not_spectial_event_type(et id) {
			bool a = (id == SDL_EVENT_QUIT) || is_joy_event(id) || is_member_function(id) ;
			
			return a;
		}
		event_funct_t on_game_pad_button = default_event_func;
		event_funct_t on_game_pad_added = event_funct_t([this](SDL_Event e)mutable {
			m_game_pads[e.gdevice.which] = SDL_OpenGamepad(e.gdevice.which); 
			return event_t::CONTINUE; 
		}); 
		event_funct_t on_game_pad_removed = event_funct_t([this](SDL_Event e)mutable {
			SDL_CloseGamepad(m_game_pads[e.gdevice.which]);
			m_game_pads.erase(e.gdevice.which);
			return event_t::CONTINUE;
		});
		event_t handle_memeber_event(SDL_Event e) {
			if (e.type == SDL_EVENT_GAMEPAD_ADDED) return on_game_pad_added(e);
			if (e.type == SDL_EVENT_GAMEPAD_REMOVED) return on_game_pad_removed(e);
			if (e.type == SDL_EVENT_GAMEPAD_BUTTON_UP || e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) return on_game_pad_button(e); 
			return event_t::NO_FUNCT; 
		}
	
	public: 
		event_handler() = default; 
		~event_handler() = default; 

		event_handler(event_handler&) = delete;
		event_handler(event_handler&&) = delete;

		event_handler operator=(event_handler&) = delete;
		event_handler operator=(event_handler&&) = delete;

		setter_t register_event(et id, event_funct_t func) {
			if (id == SDL_EVENT_QUIT) return std::unexpected(std::string("special Event Passed"));
			if (!m_events.empty()) {
				for (const auto& evt : m_events) {
					if (evt == id) return std::unexpected(std::string("event is already handled"));
				}
			}
			//once we get here we know that the event is not registared 
			m_events.emplace_back(id, func);
			return std::string("added");
		}
		setter_t set_event_handle(et id, event_funct_t funct) {
			
			size_t index = 0; 
			auto num_events = m_events.size(); 
			
			for (size_t x = 0; x <=  num_events; ++x ) {
				//maybe this should add it? something to think about... 	
				if(x == num_events ) return std::unexpected("cannont change callback for unregistered callback");

				auto current = m_events[x]; 
				if (current == id) {
					index = x;
					break; 
				}
			}
			m_events[index].funct = funct; 
			return std::string("set");	
		}
		
		void set_game_pad_button_callback(event_funct_t funct) {
			on_game_pad_button = funct; 
		}
		
		event_t do_event(SDL_Event e) {
			
			if (is_member_function((et)e.type)) {
				return handle_memeber_event(e); 
			}

			if (is_joy_event((et)e.type)) return event_t::JOY_SITCK_EVENT;
			if (e.type == SDL_EVENT_QUIT) return event_t::QUIT; 
			
			for (const auto& evnt : m_events) {
				if (evnt == e.type) {
					return evnt.funct(e);
				}
			}
			return event_t::NO_FUNCT;
		}
		//I don't know when this would be used but good to have 
		bool event_is_regisitered(et id) const {
			for (const auto &t : m_events) {
				if (t == id) return true;
			}
			return false;
		}
		//NOTE(skc): this is not good...
		//but I don't think that there's another way to do this. 
		std::expected<SDL_Gamepad*, std::string> get_game_pad(Uint32 id) {
			try {
				auto t = m_game_pads.at(id); 
				return t; 
			}
			catch (std::out_of_range) {
				return std::unexpected("no game pad with passed in ID"); 
			}
		}
	};
}