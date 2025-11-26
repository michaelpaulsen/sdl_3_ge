
#define NOMINMAX

#include <algorithm>
#include <chrono>
#include <format>
#include <print>
#include <thread>
#include "header/math/Math.hpp"
#include "header/GE/Events.hpp"
#include "header/GE/imgui_window.hpp"
#include "header/GE/window.hpp"
#include "header/ini/settings.hpp"
#include "header/main.hpp"
#include "header/math/Vector.hpp"
#include "header/filesys/windows/folder_listener.hpp"
#include "header/GE/font.hpp"
#include "header/format/printFmtutils.hpp"

auto get_current_day() {
	namespace chr = std::chrono;
	using sysclk = chr::system_clock;
	return chr::floor<chr::days>(chr::current_zone()->to_local(sysclk::now()));
}
auto now_as_lt(){
	namespace chr = std::chrono;
	using sysclk = chr::system_clock;
	return chr::current_zone()->to_local(sysclk::now());
}
double drand() {
	return (double)rand() / (double)RAND_MAX; 
}
 
int main(main_info_t info) {
	bool do_fade = false; 
	int fade_frames{};
	SKC::GE::c_t text_alpha = 255; 
	SKC::GE::c_t bg_alpha = 128;
	SKC::INI::settings_t settings("./settings.ini");

#pragma region set the default values for the program
	SKC::Math::Vect2i window_size = {
		settings.or_else("window", "width", 1080),
		settings.or_else("window", "height", 720)
	};

	bool do_render{settings.or_else("misc", "render", info.args.get_as("render", true))}; 
	auto window_name{ settings.or_else<std::string> ("window", "name", "UNTITLED WINDOW") };
	
	
	Uint64 frame{};

	
	SKC::GE::event_handler<false> event_handler{};
	int start_hour = settings.or_else("time", "hour", 9);
	int start_minute = settings.or_else("time", "minute", 0);
	int start_second = settings.or_else("time", "second", 0); 

#pragma endregion
	
	if (!do_render) return 1; 
	//TODO(skc) : handle window settings in main.hpp 
	auto window = SKC::GE::window(
		window_name,
		window_size.x,
		window_size.y,
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_TRANSPARENT
	);
	


	//key combo stuff
	char LFK = 0;
	window.set_background_color(SKC::GE::color(0, 0, 0, 128));
	std::string tmessage = ""; 
	auto font = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 128); 
	auto font_outline = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 128); 
	auto font_small = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 64); 
	auto font_small_outline = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 64); 
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
				settings.reload();
				std::println("reloading settings ");
				start_hour = settings.or_else("time", "hour", 9);
				start_minute = settings.or_else("time", "minute", 0);
				start_second = settings.or_else("time", "second", 0);
				window_name = settings.or_else<std::string>("window", "name", "UNTITLED WINDOW");
				window.set_title(window_name); 
				return; 
			}

			std::println("path : {} action {}", info.fname.string(), info.action);
		},
		SKC::file_api::defnotfilt,
		true
	);
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
#pragma endregion
		window.set_background_color(SKC::GE::color(0, 0, 0, bg_alpha));
		window.clear();
		
		window.set_render_scale(1);

		if (!(frame % 30)) {
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
			if (hrs < 0) {
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
			}
		}
		if (do_fade) {
			if (fade_frames > 60) {
				if (text_alpha > 0) {
					--text_alpha;
				}
				if (frame % 2 == 0 && bg_alpha > 0) {
					--bg_alpha;
				}
			}
			++fade_frames;
		}
		if (!do_fade) {
			if (text_alpha < 255) {
				++text_alpha;
			}
			if (bg_alpha < 128 and frame % 2 == 0) {
				++bg_alpha;
			}
			fade_frames = 0; 
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
				window.render_text(std::format("stream title \n{}", stream_title), *font_small_outline, outline_font_settings);
				window.render_text(std::format("stream title \n{}", stream_title), *font_small, font_settings);
			}
			else {}

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