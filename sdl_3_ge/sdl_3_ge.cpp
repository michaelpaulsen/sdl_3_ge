
#define NOMINMAX
#include <chrono>
#include <cmath>
#include <filesystem>
#include <format>
#include <print>
#include <string>
#include <thread>
#include <vector>

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

#include "header/filesys/windows/folder_listener.hpp"

#include "header/format/printFmtutils.hpp"

#include "header/GE/Color.hpp"
#include "header/GE/Events.hpp"
#include "header/GE/font.hpp"
#include "header/GE/font_options.hpp"
#include "header/GE/window.hpp"

#include "header/ini/settings.hpp"

#include "header/main.hpp"
#include "header/math/Math.hpp"
#include "header/math/rand.hpp"
#include "header/math/Vector.hpp"

auto now_as_lt() {
	namespace chr = std::chrono;
	using sysclk = chr::system_clock;
	return chr::current_zone()->to_local(sysclk::now());
}
auto get_current_day() {
	namespace chr = std::chrono;
	using sysclk = chr::system_clock;
	return chr::floor<chr::days>(now_as_lt());
}

void get_offset_time(int& hro, int& mno, int sco) {
	namespace chr = std::chrono;
	auto cur_time = chr::hh_mm_ss{ now_as_lt() - get_current_day() };
	hro += (int)(cur_time.hours()  .count());
	mno += (int)(cur_time.minutes().count());
	if (sco == 0) {
		--mno; 
		sco = 59; 
	}
	else {
		sco += (int)(/*60l - */cur_time.seconds().count());
	}
}; 

int main(main_info_t info) {
	SKC::GE::RNG<size_t> t{}; 
	
	bool show_game_play = false, do_fade = false;
	SKC::GE::c_t text_alpha = 255; 
	SKC::GE::c_t bg_alpha = 128;
	
	SKC::INI::settings_t settings("./settings.ini");

#pragma region set the default values for the program
	SKC::Math::Vect2i window_size = {
		settings.or_else("window", "width", 1080),
		settings.or_else("window", "height", 720)
	};
	int start_hour = {}, start_minute = {}, start_second = {}, r_h{}, r_m{}, r_s{};
	
	std::string window_name{}, tmessage{};
	auto reload_settings = [&]() mutable {
		settings.reload();
		static bool has_init_timer = false;
		bool is_countdown = settings.or_else("time", "is_rel", false);
		bool reset_countdown = settings.or_else("time", "reset", false);
		window_name = settings.or_else<std::string>("window", "name", "UNTITLED WINDOW");
		show_game_play = settings.or_else("draw", "show game play", false);
		
		if (is_countdown) {
			bool should_update_timer{};
			if (not has_init_timer) {
				r_h = settings.or_else("time", "hour", 9);
				r_m = settings.or_else("time", "minute", 0);
				r_s = settings.or_else("time", "second", 0);
				should_update_timer = true;
			}
			else {
				should_update_timer = (
					not (r_h == settings.or_else("time", "hour", 9)) or
					not (r_m == settings.or_else("time", "minute", 0)) or
					not (r_s == settings.or_else("time", "second", 0))
				);
			}

			 
			if (reset_countdown or (should_update_timer)) {
				start_hour   = r_h = settings.or_else("time", "hour"  , 9);
				start_minute = r_m = settings.or_else("time", "minute", 0);
				start_second = r_s = settings.or_else("time", "second", 0);
				get_offset_time(start_hour, start_minute, start_second);
				has_init_timer = true;
			}
		}
		else {
			start_hour = settings.or_else("time", "hour", 9);
			start_minute = settings.or_else("time", "minute", 0);
			start_second = settings.or_else("time", "second", 0);
		}
	};

	
#pragma endregion


	SKC::GE::event_handler<false> event_handler{};
	Uint64 frame{};

	auto bgimgpaths = std::vector<std::filesystem::path>{};
	try{
	for (const auto& dirent : std::filesystem::directory_iterator("./img/backgrounds")) {
		if (dirent.path().extension() == ".png") {
			bgimgpaths.emplace_back(dirent.path());
		}
	}
	std::println("[INFO] IMAGES LOADED : {}", bgimgpaths.size());
	}
	catch (const std::filesystem::filesystem_error& e) {
		std::println("[ERROR] {}", e.what()); 
		return -1;
	}
	//TODO(skc) : handle window settings in main.hpp 
	auto window = SKC::GE::window(
		window_name,
		window_size.x,
		window_size.y,
		SDL_WINDOW_RESIZABLE   |
		SDL_WINDOW_TRANSPARENT |
		SDL_WINDOW_HIDDEN
	);



	
	char LFK = 0;
	
#pragma region font stuff 
	auto font = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 128); 
	auto font_outline = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 128); 
	auto font_small = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 64); 
	auto font_small_outline = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 64); 
	
#pragma endregion 
#pragma region background stuff
	std::vector<size_t> bacground_image_ids{}; 
	bool is_first_file = true; 
	for (const auto& pth : bgimgpaths) {
		if (is_first_file) {
			t.min = window.create_texture_from_path(pth);
			t.max = t.min;
			is_first_file = false; 
			continue; 
		}
		t.max = window.create_texture_from_path(pth);

	}
	auto main_drawing = t.get_random();
#pragma endregion 
	

	window.show();
	reload_settings();

	font_outline.make_out_line_font(2); 
	font_small_outline.make_out_line_font(2); 
	
	if (!font || !font_small) {
		return -1;
	}
	
	auto fl = std::jthread(SKC::file_api::watch_directory,
		std::filesystem::path("./"),
		//NOTE(skc): I know that this is bad practice get off my back. 
		[&](SKC::file_api::fs_change_info_t info) mutable {
			if (info.fname == "settings.ini") {
				reload_settings(); 
				window.set_title(window_name);
				return; 
			}

			std::println("path : {} action {}", info.fname.string(), info.get_action());
		},
		SKC::file_api::defnotfilt,
		true
	);
	bool resize_window = true; 
	bool set_borderless = true; 
//=========MAIN LOOP START =========
	while (true) {
		
		window.start_of_frame(); 
		LFK = event_handler.get_last_key() ? event_handler.get_last_key() : LFK; 
		event_handler.pollevents(); 
		//if the event handler has a quit event then we need to exit the main loop
		if (event_handler.quit()) break;

		//handle window resize events
		if (event_handler.window_resized()) {
			window.update_window_size(); 
			window_size = window.get_window_dimentions();
			std::println("\n\nwindow resized to {}x{}", window_size.x, window_size.y);
		}
		
#pragma region accept user input 
		auto f2_key = event_handler.get_function_key_state(2); 
		auto f3_key = event_handler.get_function_key_state(3); 
		if (f2_key.down and resize_window) {
			auto success = window.set_window_full_screen(
				not window.is_window_fullscreen()
			);
			if (not success) {
				std::println("[SDL ERROR] -> {}", SDL_GetError());
			}
			resize_window = false; 
		}
		if (f3_key.down and set_borderless) {
			auto success = window.set_window_border(
				not window.is_window_bordered()
			);
			if (not success) {
				std::println("[SDL ERROR] -> {}", SDL_GetError());
			}
			set_borderless = false; 
		}
		if (not f2_key.down) resize_window = true;
		if (not f3_key.down) set_borderless = true;
#pragma endregion
		window.set_background_color(SKC::GE::color(0, 0, 0, bg_alpha));
		window.clear();
		
		window.set_render_scale(1);
		
		if (not (frame % 30)) {
			namespace chr = std::chrono;
			using sysclk = chr::system_clock;
			auto cur_time = chr::hh_mm_ss{ now_as_lt() - get_current_day() };
			auto hrs = start_hour - cur_time.hours().count();
			auto mns = (start_minute - cur_time.minutes().count());
			auto sec = (start_second - cur_time.seconds().count());
			
			if (mns < 0) {
				--hrs; 
				mns += 59;
			}
			if (sec < 0) {
				--mns; 
				sec += 60;
			}
			if (mns < 0 or hrs < 0) {
				do_fade = true; 
			}
			else {
				do_fade = false; 
			}
		
			//TODO(skc): make this message customizable via the ini file
			if (!do_fade) {
				tmessage = std::format("starting soon\n {:02}:{:02}:{:02}",  hrs, mns, sec);
			}
			else {
				tmessage = "stream starting soon";
				/*if (not frame == 0) {
						--text_alpha;
						if(frame % 2 == 0)
						--bg_alpha;
				}*/
			}
		}
		if (frame and not (frame % 300)) {
			main_drawing = t.get_random();
		}
		
		//window.set_texture_alpha_mod(main_drawing, bg_alpha);
		window.draw_texture(main_drawing);
		window.set_draw_color(0, 255, 0,0); 
		auto posx = 0.5f, height = 0.5f; 
		if (show_game_play) {
			window.fill_rectangle(SDL_FRect{ (float)window.from_normilzed_width(posx),0,(float)window.from_normilzed_width(1-posx), (float)window.from_normilzed_height(height) });
		}
		if (text_alpha > 0) {
			auto font_settings = SKC::GE::font_options{};
			font_settings.x = (float)window_size.x / 2;
			font_settings.y = (float)window_size.y / 2;
			font_settings.line_separator = SKC::GE::font_options::LINE_SEPARATOR_NEWLINE;
			font_settings.text_alignment = SKC::GE::font_options::TEXT_ALIGNMENT_CENTER;
			font_settings.line_alignment = SKC::GE::font_options::LINE_ALIGNMENT_CENTER;
			font_settings.anchor_point = SKC::GE::font_options::AP_CENTER;
			font_settings.color = SKC::GE::color(
				(SKC::GE::c_t)SKC::Math::map(sin((double)frame / 100.), -1, 1, 128, 255),
				(SKC::GE::c_t)SKC::Math::map(sin((double)frame / 250.), -1, 1, 128, 255),
				(SKC::GE::c_t)SKC::Math::map(sin((double)frame / 121.), -1, 1, 128, 255),
				text_alpha
			);
			auto outline_font_settings = font_settings;
			outline_font_settings.color = SKC::GE::color(0, 0, 0, text_alpha); 

			window.render_text(tmessage, *font_outline, outline_font_settings);
			window.render_text(tmessage, *font, font_settings);
			
			
			
			font_settings.y = 64;
			font_settings.x = 10;
			font_settings.line_alignment = SKC::GE::font_options::LINE_ALIGNMENT_TOP;
			font_settings.anchor_point = SKC::GE::font_options::AP_TOP_LEFT;
			
			outline_font_settings = font_settings;
			outline_font_settings.color = SKC::GE::color(0, 0, 0, text_alpha);

			auto stream_title = settings.or_else<std::string>("stream", "title", "");
			auto pre_title_text = settings.or_else<std::string>("stream", "action", "STREAM TITLE");
			
			if (not stream_title.empty()) {
				auto text = std::format("{}\n{}", pre_title_text, stream_title);

				window.render_text(text, *font_small_outline, outline_font_settings);
				window.render_text(text, *font_small, font_settings);
			}
			else {
				window.render_text(pre_title_text, *font_small_outline, outline_font_settings);
				window.render_text(pre_title_text, *font_small, font_settings);
			}

		}
		window.present();
		window.wait_for_frame();
		std::print("frame rate {}{}\r", 1.f / ((float)(window.delta_time()) / 1000),SKC::fmt::clear_line_after);
		++frame;
#pragma endregion
	}
	fl.request_stop(); 
	return 0;
}