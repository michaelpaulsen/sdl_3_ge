#pragma once
#include <SDL3/SDL.h> 
#include <string>
#include <unordered_map>
#include <optional>
namespace SKC::GE::controller {

	//this is here to keep the code dry. I forsee that most of the apis in this namespace 
	//are going to use this in some way... 
	// maybe this should be in the top-level namespace? 
	using opt_string = std::optional<std::string>; 
	
	//SA : https://wiki.libsdl.org/SDL3/SDL_GamepadButtonLabel
	//for some reason there's not an SDL function that does this? 
	//if which is an ID of controller face button it returns an optional containing the name of that
	//button otherwise it returns an empty optional.
	//arguably this should be a layer of abstraction higher! ... 
	//RANT(skc): there's no reason that SDL_GetGamepadButtonLabel couldn't return a char* instead of
	opt_string get_controller_face_button_name(SDL_Gamepad* gp, Uint8 which) {
		
		//TODO(skc): exstend this to return the names of the hat and axises
		auto label = SDL_GetGamepadButtonLabel(gp, (SDL_GamepadButton)which); 
		switch (label) {
				case SDL_GAMEPAD_BUTTON_LABEL_A: return "A"; 
				case SDL_GAMEPAD_BUTTON_LABEL_B: return "B"; 
				case SDL_GAMEPAD_BUTTON_LABEL_X: return "X"; 
				case SDL_GAMEPAD_BUTTON_LABEL_Y: return "Y"; 

				case SDL_GAMEPAD_BUTTON_LABEL_CROSS: return "Cross";
				case SDL_GAMEPAD_BUTTON_LABEL_CIRCLE: return "Circle"; 
				case SDL_GAMEPAD_BUTTON_LABEL_SQUARE: return "Square"; 
				case SDL_GAMEPAD_BUTTON_LABEL_TRIANGLE: return "Triangle";
				default: return {};
		}
	}	
}
