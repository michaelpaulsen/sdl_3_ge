#define NOMINMAX
#include <filesystem>
#include <print>
#include <string>
#include <thread>
#include <vector>


#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include "header/filesys/windows/folder_listener.hpp"
#include "header/GE/Events.hpp"
#include "header/GE/window.hpp"
#include "header/ini/settings.hpp"
#include "header/math/Vector.hpp"

#include "header/main.hpp"

static bool should_restart = false;

int main([[maybe_unused]] main_info_t info) {
	//SKC::GE::RNG<size_t> t{}; 
	
	
	SKC::INI::settings_t settings("./settings.ini");
#pragma region set the default values for the program
	SKC::Math::Vect2i window_size = {
		settings.or_else("window", "width", 1080),
		settings.or_else("window", "height", 720)
	};

	std::string window_name{ settings.or_else<std::string>("window", "name", "UNTITLED WINDOW") };
	auto reload_settings = [&]() mutable {
		settings.reload();
		static bool has_init_timer = false;
		window_name = settings.or_else<std::string>("window", "name", "UNTITLED WINDOW");
	};

	


	SKC::GE::event_handler<false> event_handler{};
	Uint64 frame{};

	//=========WINDOW SETUP=========
	//TODO(skc) : handle window settings in main.hpp 
	auto window = SKC::GE::window(
		window_name,
		window_size.x,
		window_size.y,
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_TRANSPARENT
	);
	std::vector<size_t> background_image_ids{};

	auto fl = std::jthread(SKC::file_api::watch_directory,
		std::filesystem::path("./"),
		//NOTE(skc): I know that this is bad practice get off my back. 
		[&](SKC::file_api::fs_change_info_t info) mutable {
			if (info.fname == "settings.ini") {
				reload_settings();
				return;
			}

			std::println("path : {} action {}", info.fname.string(), (int)info.action);
		},
		SKC::file_api::defnotfilt,
		true
	);
#pragma endregion
	SDL_SetHint(SDL_HINT_GPU_DRIVER, "Vulkan"); 
	std::println("WORKING DIR : {}", std::filesystem::current_path().string());
	auto vertex_shader = window.create_vetex_shader(
		"./samples/shaders/vert.spv", SDL_GPU_SHADERFORMAT_SPIRV); 
	auto fragment_shader = window.create_fragment_shader(
		"./samples/shaders/frag.spv", SDL_GPU_SHADERFORMAT_SPIRV);
	window.bind_gpu_device(); 


//=========MAIN LOOP START =========
	while (true) {
		window.start_of_frame();
		//if the event handler has a quit event then we need to exit the main loop
		event_handler.pollevents();
		if (event_handler.quit()) break;
		
		//handle window resize events
		if (event_handler.window_resized()) {
			window.update_window_size();
			window_size = window.get_window_dimentions();
			std::println("\n\nwindow resized to {}x{}", window_size.x, window_size.y);
		}

#pragma region accept user input 
		
#pragma endregion
#pragma region draw section 
		window.clear();

		window.present(); 
#pragma endregion
	}
	fl.request_stop(); 
	return 0;
}