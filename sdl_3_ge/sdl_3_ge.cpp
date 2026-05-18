#define NOMINMAX
#include <chrono>
#include <filesystem>
#include <format>
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
#include "header/math/rand.hpp"
#include "header/math/Vector.hpp"
#include "header/GE/tile_map.hpp"

#include "header/main.hpp"

enum class TILES {
	NONE = 0, 
	X,
	O
};

int main([[maybe_unused]] main_info_t info) {

#pragma region set the default values for the program
	//SKC::GE::RNG<size_t> t{}; 
	uint8_t turn = 0; 
	bool show_game_play = false, do_fade = false;
	SKC::GE::c_t text_alpha = 255,bg_alpha = 255;
	
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
	std::vector<size_t> background_image_ids{};

#pragma region font stuff 
#pragma endregion 
#pragma endregion set the default values for the program
	auto fl = std::jthread(SKC::file_api::watch_directory,
		std::filesystem::path("./"),
		//NOTE(skc): I know that this is bad practice get off my back. 
		[&](SKC::file_api::fs_change_info_t info) mutable {
			if (info.fname == "settings.ini") {
				reload_settings();
				//window.set_title(window_name);
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
	auto main_drawing = SKC::GE::choose_from(background_image_ids);
	window.draw_texture(main_drawing);
	window.show();
	const int z = 10; 
	auto tile_map = SKC::GE::tile_map<TILES>(
		z,z,TILES::NONE,32
	); 
	auto tile_size = tile_map.tile_size();

	tile_map.tile_size_from_window_size(window_size); 
	auto font = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 24); 
//=========MAIN LOOP START =========
	while (true) {
		window.start_of_frame();
		event_handler.pollevents();
		//if the event handler has a quit event then we need to exit the main loop
		if (event_handler.quit()) break;
		//handle window resize events
		if (event_handler.window_resized()) {
			window.update_window_size();
			window_size = window.get_window_dimentions();
			//tile_map.tile_size_from_window_size(window_size); 
		}
#pragma region accept user input 
		if (event_handler.get_key_state('r').down) {
			tile_map.fill(TILES::NONE); 
			turn = 0; 
		}
#pragma endregion
		window.clear(); 
		auto mpos = event_handler.mouse_position(); 
		
		tile_size += event_handler.scroll_wheel_pos().y; 
		tile_map.set_tile_size(tile_size); 
		auto mouse_tile = tile_map.to_tile_map_pos(mpos.x, mpos.y );
		auto dv = tile_map.get_offset(); 
		auto woff = dv.x; 
		auto hoff = dv.y; 
		tile_map.offset_px(event_handler.drag_vector()); 
		for (size_t y = 0; y < z; ++y) {
			for (size_t x = 0; x < z; ++x) {
								
				double mx = (x * tile_size)+woff, my = (y* tile_size) + hoff;
				window.set_draw_color(128, 128, 128); 
				window.fill_rectangle(mx, my, tile_size, tile_size);
				auto& tile = tile_map.at(x, y); 

				if (mouse_tile.x == x && mouse_tile.y == y 
					// && event_handler.get_mouse_button_state(1).down
				) {
						
					
					
					if ( 
						!event_handler.is_dragging() && 
						event_handler.get_mouse_button_state(1).down &&
						tile == TILES::NONE
					) {
						tile = (turn++) % 2 == 0 ? TILES::X : TILES::O;
					}
					window.set_draw_color(255, 0, 0); 
					window.fill_rectangle(mx, my, tile_size, tile_size); 
				}
				if (tile == TILES::X) {
					window.set_draw_color(255, 64, 128); 
					float cx = woff, nx{};
					float cy = hoff, ny{};
					cx += x * tile_size;
					nx = cx + tile_size;

					cy += y * tile_size;
					ny = cy + tile_size;
					
					window.draw_line(cx, cy, nx, ny);
					window.draw_line(nx, cy, cx,ny);
				}
				if (tile == TILES::O) {
					double cx = (x * tile_size) + woff + (tile_size/ 4.),
						cy = tile_size * y + hoff,
						w = tile_size/ 2.;
					window.set_draw_color(64, 255, 64);
					window.draw_rectangle(cx, cy, w, tile_size); 


				}
				window.set_draw_color(64, 64, 255);
				window.draw_rectangle(mx, my, tile_size, tile_size);

			}
		}

		window.fill_rectangle(mpos.x-10, mpos.y-10, 20, 20); 
		auto font_options = SKC::GE::font_options{


		}; 
		font_options.color = { 0,0,0 }; 
		font_options.line_seperator = font_options.LINE_SEPARATOR_NEWLINE; 
		//font_options.width = 100; 
		//font_options.height = 100; 
		window.render_text(
			std::format("mouse drag started {}\nmouse draging {}\n mouse wheel {}",
				event_handler.has_started_dragging(), event_handler.is_dragging(), event_handler.scroll_wheel_pos()
			), *font, font_options); 
		window.present(); 
#pragma endregion
	}
	fl.request_stop(); 
	return 0;
}