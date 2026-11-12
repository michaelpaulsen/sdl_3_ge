#pragma once
namespace SKC::GE::types {
	enum struct full_screen_state_change_t : unsigned char {
		NO_CHANGE,
		EXITED_FULLSCREEN,
		ENTERED_FULL_SCREEN

	};
	enum struct drop_event_data_type_t : unsigned char {
		NO_DROPPED_DATA,
		FILE_DATA,
		TEXT_DATA

	};
	enum class special_keys : unsigned {
		BACKSPACE,
		RETURN,
		INSERT,
		HOME,
		END,
		PAGEDOWN,
		PAGEUP,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		ESC = SDLK_ESCAPE,
		DEL = SDLK_DELETE,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		LAST
	};
	enum class E_EVENT_IDS : unsigned {
		MOUSE_M_UP = (unsigned)special_keys::LAST,
		MOUSE_M_DOWN,
		MOUSE_MOTION,
		MOUSE_CLICKED,
	};
}