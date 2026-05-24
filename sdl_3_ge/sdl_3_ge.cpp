#define NOMINMAX
#include <filesystem>
#include <print>
#include <string>
#include <thread>
#include <vector>

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>
#include "header/filesys/windows/folder_listener.hpp"
#include "header/GE/Color.hpp"
#include "header/GE/Events.hpp"
#include "header/GE/font.hpp"
#include "header/GE/font_options.hpp"
#include "header/GE/imgui_window.hpp"
#include "header/GE/window.hpp"
#include "header/ini/settings.hpp"
#include "header/math/Vector.hpp"
#include "header/main.hpp"




int main([[maybe_unused]] main_info_t info) {

#pragma region set the default values for the program

	SKC::INI::settings_t settings("./settings.ini");
	SKC::Math::Vect2i window_size = {
		settings.or_else("window", "width", 1080),
		settings.or_else("window", "height", 720)
	};
	std::string window_name{ settings.or_else<std::string>("window", "name", "UNTITLED WINDOW") }, tmessage{};
	auto reload_settings = [&]() mutable {
		};
	SKC::GE::event_handler<true> event_handler{};
	Uint64 frame{};

	//=========WINDOW SETUP=========

	//TODO(skc) : handle window settings in main.hpp 
	auto window = SKC::GE::imgui_window(
		window_name,
		window_size.x,
		window_size.y,
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_TRANSPARENT |
		SDL_WINDOW_HIDDEN
	);

#pragma region font stuff 
#pragma endregion 
#pragma endregion set the default values for the program
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

#pragma region splash screen / loading stuff
	{
		auto sc_font = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 12);
		auto splash_screen = SKC::GE::window(
			"Loading...",
			600,
			400,
			SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_TRANSPARENT
		);
		splash_screen.set_background_color(SKC::GE::color(255, 0, 0,0));
		splash_screen.clear();
		splash_screen.render_text("loading\nSKCGE\nV0.24.3", *sc_font, {
			.x = 300,
			.y = 200,
			.color = SKC::GE::color(255,0,255),
			.line_alignment = SKC::GE::font_options::LINE_ALIGNMENT_CENTER,
			.line_seperator = SKC::GE::font_options::LINE_SEPARATOR_NEWLINE,
			.anchor_point = SKC::GE::font_options::AP_CENTER
			});

		splash_screen.present();
#pragma region background stuff
		reload_settings();
		window.set_frame_rate(30);
#pragma endregion 
	}
#pragma endregion 
	window.show();
	//=========MAIN LOOP START =========
	while (true) {
		event_handler.pollevents(); 
		window.clear();
		if (event_handler.quit()) break;
		window.present(); 
		++frame;
		window.wait_for_frame();
#pragma endregion
	}
	fl.request_stop();
	return 0;
}