
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
#include <imgui.h>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include "header/filesys/windows/folder_listener.hpp"

#include "header/format/printFmtutils.hpp"

#include "header/GE/Color.hpp"
#include "header/GE/Events.hpp"
#include "header/GE/font.hpp"
#include "header/GE/font_options.hpp"
#include "header/GE/window.hpp"
#include "header/GE/imgui_window.hpp"

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


int main([[maybe_unused]] main_info_t info) {
	//SKC::GE::RNG<size_t> t{}; 
	
	bool show_game_play = false, do_fade = false;
	SKC::GE::c_t text_alpha = 255,bg_alpha = 255;
	
	SKC::INI::settings_t settings("./settings.ini");
	auto stream_title = settings.or_else<std::string>("stream", "title", "");
	auto pre_title_text = settings.or_else<std::string>("stream", "action", "STREAM TITLE");
#pragma region set the default values for the program
	SKC::Math::Vect2i window_size = {
		settings.or_else("window", "width", 1080),
		settings.or_else("window", "height", 720)
	};
	int start_hour = {}, start_minute = {}, start_second = {}, r_h{}, r_m{}, r_s{};
	char LFK = 0;
	int dir = 0;
	bool is_countdown = settings.or_else("time", "is_rel", false);
	bool reset_countdown = settings.or_else("time", "reset", false);

	std::string window_name{ settings.or_else<std::string>("window", "name", "UNTITLED WINDOW") }, tmessage{};
	auto reload_settings = [&]() mutable {
		settings.reload();
		reset_countdown = settings.or_else("time", "reset", false);
		static bool has_init_timer = false;
		window_name = settings.or_else<std::string>("window", "name", "UNTITLED WINDOW");
		show_game_play = settings.or_else("draw", "show game play", false);
		stream_title = settings.or_else<std::string>("stream", "title", "");
		pre_title_text = settings.or_else<std::string>("stream", "action", "STREAM TITLE");\
		start_hour = settings.or_else("time", "hour", 9);
		start_minute = settings.or_else("time", "minute", 0);
		start_second = settings.or_else("time", "second", 0);
		is_countdown = settings.or_else("time", "is_rel", false);
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
	auto font = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 128);
	auto font_outline = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 128);
	auto font_small = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 64);
	auto font_small_outline = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 64);
	font_outline.make_out_line_font(2);
	font_small_outline.make_out_line_font(2);
#pragma endregion 


	bool resize_window = true;
	bool set_borderless = true;

	bool show_imgui = false, real_imgui_satus = false;
	bool f1_key_down = false;

	auto test_img_tid = window.create_texture_from_path("./img/test.bmp");
	window.set_texture_scale_mode(test_img_tid, SDL_SCALEMODE_NEAREST);

	auto sqsizeie = window.get_window_dimentions() / 10;
#if true 
	auto mtxt_test_size = window.get_window_dimentions() / 5;
#else
	auto mtxt_test_size = window.get_window_dimentions();
#endif
	auto mtxt_test = window.create_modifible_texture(mtxt_test_size.x, mtxt_test_size.y);
	window.set_texture_scale_mode(mtxt_test, SDL_SCALEMODE_NEAREST);
	window.set_texture_blend_mode(mtxt_test, SDL_BLENDMODE_BLEND);
	SKC::Math::Vect2i lmpos{ 0,0 };
	

	if (!font || !font_small) {
		return -1;
	}
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
	{

		auto sc_font = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 48);

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

		try {
			bool is_first_file = true;

			for (const auto& dirent : std::filesystem::directory_iterator("./img/backgrounds")) {
				auto &pth = dirent.path();
				if (pth.extension() == ".png") {
					background_image_ids.emplace_back(window.create_texture_from_path(pth));
				}
			}
		}
		catch (const std::filesystem::filesystem_error& e) {
			std::println("[error] {}", e.what());
			return -1;
		}
		reload_settings();
		window.set_frame_rate(30);
#pragma endregion 
		

	}
	auto main_drawing = *SKC::GE::choose_from(background_image_ids);
	window.draw_texture(main_drawing);
	window.show();

//=========MAIN LOOP START =========
	while (true) {
		window.start_of_frame();
		LFK = event_handler.get_last_key() ? event_handler.get_last_key() : LFK;
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
		if (event_handler.get_key_state('w').down)  dir = 0;
		if (event_handler.get_key_state('d').down)  dir = 1;
		if (event_handler.get_key_state('s').down) dir = 2;
		if (event_handler.get_key_state('a').down) dir = 3;
		if (event_handler.get_function_key_state(1).down) {
			if (not f1_key_down) {
				real_imgui_satus = not show_imgui;
			}
			f1_key_down = true;
		}
		else {
			f1_key_down = false; 
		}
#pragma endregion

		//window.set_background_color(SKC::GE::color(0, 0, 0, bg_alpha));
		window.clear();
		window.set_render_target(0);
		window.set_render_scale(1);
		window. update_io_display_size();
		window.im_start_frame();
		if (show_imgui) {
			ImGui::Begin("Settings", &real_imgui_satus, ImGuiWindowFlags_AlwaysAutoResize);
			
			ImGui::SeparatorText("noise settings");
			ImGui::Text(std::format("loaded background images {}", background_image_ids.size()).c_str());
			ImGui::Text(std::format("current background {}", main_drawing).c_str());
		}
		
		

		//if(frame% int(window.get_frame_rate()/4))
		{
			
			window.set_render_target(mtxt_test);
			window.draw_texture(main_drawing);

			srand((uint32_t)(time(NULL)));
			static float xdx = 0;
			static float xoffset = (float)rand(), yoffset = (float)rand();
			static int y_stepie = 10;
			static float scale = 0.097f;//1.f / 50.f;
			if (show_imgui) {
				ImGui::DragFloat("scale", &scale, 0.0001f, 0.0001f, 0.1f);
				ImGui::DragInt("pixel size", &y_stepie, .5, 1,10);
			}
			skc_Vect2d txt_scale_factor =
				skc_Vect2d(window.get_texture_size(mtxt_test) / sqsizeie);
			skc_Vect2d mp_scale =
				skc_Vect2d(window.get_window_dimentions()) / window.get_texture_size(mtxt_test);

			auto mposs = event_handler.mouse_position()/ mp_scale / txt_scale_factor;
			static int cr = 5; 
			static int wr = 64;
			static bool do_mouse_UDF = false; 
			if (show_imgui) {
				ImGui::SliderInt("inner radius", &cr, 0, 100);
				ImGui::SameLine();  
				ImGui::SliderInt("outer radius", &wr, 0, 100);
				ImGui::Checkbox("mouse signed distance feild", &do_mouse_UDF); 
			}
			static auto color = SKC::GE::color(255);//SKC::GE::color(rand()%255, rand() % 255, rand() % 255);
			//if(frame % 200 != 0) color = SKC::GE::color(rand() % 255, rand() % 255, rand() % 255);
			auto cr2 = cr * cr, wr2 = wr* wr;
			//if (cr > wr) cr = wr; 
			for (float x = 0; x < sqsizeie.x; x += y_stepie) {
				for (float y = 0; y < sqsizeie.y; y += y_stepie) {
					auto a = 1.;
					auto real_x = x + xoffset; 
					auto real_y = y + yoffset; 
					real_x *= scale; 
					real_y *= scale; 
					if (do_mouse_UDF) {
						a = (skc_Vect2d{ x,y } - mposs).len2();
						a = SKC::Math::clamped_map(a, cr2, wr2, 0, 1); 
						a *= SKC::Math::perlin(real_x, real_y);
						a = SKC::Math::map(a, 0, 1, 0, 255);
					}
					else {
						a = SKC::Math::perlin(real_x, real_y);
						a = SKC::Math::map(a, 0, 1, 0, 255); 
					}
					window.set_draw_color(color.r, color.g, color.b,(uint8_t)a);
					auto r = 255*SKC::Math::perlin((x + xoffset) * scale, (y + yoffset) * scale);
					//window.set_draw_color(SKC::GE::color(a));
					
					
					window.fill_rectangle(
						(float)       x * (float)txt_scale_factor.x,
						(float)       y * (float)txt_scale_factor.y,
						(float)y_stepie * (float)txt_scale_factor.x,
						(float)y_stepie * (float)txt_scale_factor.y
					);
				}
				
			}
			

			static float spe2 = 1;
			xoffset += spe2;
			yoffset += spe2 / 16;
			xdx++;
			if (show_imgui) {

				ImGui::SliderFloat("noise scroll speed", &spe2, 0.001f, 10);
			}
		}
		
		window.set_render_target(0);
		window.set_texture_alpha_mod(mtxt_test, bg_alpha);
		window.draw_texture(mtxt_test);

		
		

		/*auto s = std::fabs(std::sin(double(frame) / 20)), l = std::fabs(std::sin(double(frame) / 300));
		auto tclr = SKC::GE::color_hsl(std::fmod(float(frame), 360), 1., .5 );
		if (show_imgui) {
			ImGui::SeparatorText("test settings");
			ImGui::Text("tclr h %f s %f l %f", tclr.h, tclr.s, tclr.l); 
		}
		window.set_background_color(tclr);
		window.clear();
		*/
		{
		static float secs_per_bg_change = 10.f;
		if (show_imgui) {
			ImGui::SeparatorText("background settings");

			ImGui::SliderFloat("background change interval", &secs_per_bg_change,5.f,60.f);
		}
		
			int frame_interval = (int)(window.get_frame_rate() * secs_per_bg_change);
			if (frame % frame_interval == 0) {
				main_drawing = *SKC::GE::choose_from(background_image_ids);
			}
		}
		
		
		static bool do_fade_override = false; 
		static bool do_fade_when_overriden = false; 
		if (show_imgui) {
			ImGui::Checkbox("override fade", &do_fade_override);
			if (do_fade_override) {
				ImGui::Checkbox("fade", &do_fade_when_overriden);
				if (do_fade_when_overriden) {
					ImGui::SameLine(); 
					ImGui::Text("test"); 
				}
			}
		}
		static bool should_restart = false;
		if (show_imgui) {

			ImGui::Checkbox("should restart", &should_restart);
		}
		if (not (frame % (int)window.get_frame_rate())) {
			if (is_countdown) {
				//TODO(skc) : this should check to see if a second actually passed 
				//but aside from this being fr dependant this works
				
				if (frame) {
					start_second--;
				}

				if (start_second <= -1) {
					--start_minute;
					start_second = 59; 
				}
				if (start_minute <= -1) {
					if (start_hour == 0) {
						do_fade = true; 
					}
					else {
						--start_hour; 
						start_minute = 59; 
					}
				}


				if (should_restart) {
					    start_hour = settings.or_else("time", "hour", 9);
					  start_minute = settings.or_else("time", "minute", 0);
				      start_second = settings.or_else("time", "second", 0);
					  start_second += 5;

					should_restart = false;
				}
				if(not do_fade)
				tmessage = std::format("starting soon\n {:02}:{:02}:{:02}",
					start_hour, start_minute, start_second);

			}
			else{
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
				//TODO(skc): make this message customizable via the ini file
				if (!do_fade) {
					tmessage = std::format("starting soon\n {:02}:{:02}:{:02}", hrs, mns, sec);
				}

			}
		}
		if (do_fade_override) {
			do_fade = do_fade_when_overriden;

		}
		if(do_fade) {
			tmessage = "stream starting soon";
			if(text_alpha >= 1 and not (frame % 2 )) --text_alpha;
			if (bg_alpha >= 1  and not (frame % 3))    --bg_alpha;
			if (show_imgui) {
				ImGui::Text("text_alpha:%d,bg_alpha:%d", text_alpha, bg_alpha);
			}
			
		}
		if(not do_fade and text_alpha < 255 and not (frame %2)) {
			++text_alpha;
		}
		if (not do_fade and bg_alpha < 255 and not (frame %4)) {
			++bg_alpha;
		}
		window.set_draw_color(0, 255, 0,0);
		auto posx = (float)window.from_normilzed_width(.5f), height = 0.5f;
		if (show_game_play) {
			window.set_draw_color(0, 0, 0, 0);
			
			window.fill_rectangle(
				SDL_FRect{ posx ,0, (float) window.from_normilzed_width(1.f) -posx, (float)window.from_normilzed_height(height) });
		}
		if (text_alpha > 0) {

	
			/*
			auto min_color = 0; 
			auto c = SKC::GE::color(
				(SKC::GE::c_t)SKC::Math::map(sin((double)frame / 100.), -1, 1, min_color, 255),
				(SKC::GE::c_t)SKC::Math::map(sin((double)frame / 250.), -1, 1, min_color, 255),
				(SKC::GE::c_t)SKC::Math::map(sin((double)frame / 121.), -1, 1, min_color, 255),
				text_alpha
			);
			*/
			auto c = SKC::GE::color_hsl((double)frame/1.,1,.5, text_alpha);
			if (show_imgui) {
				ImGui::Text("h %f s %f l %f ", c.h, c.s, c.l);
				static auto c2 = SKC::GE::color(255);
				c2 = c; 
				ImGui::Text("r %d g %d b %d ", c2.r, c2.g, c2.b);

			}
			auto font_settings = SKC::GE::font_options{
				.x              = (float)window_size.x / 2,
				.y              = (float)window_size.y / 2,
				.color          = c,
				.text_alignment = SKC::GE::font_options::TEXT_ALIGNMENT_CENTER,
				.line_alignment = SKC::GE::font_options::LINE_ALIGNMENT_CENTER,
				.line_seperator = SKC::GE::font_options::LINE_SEPARATOR_NEWLINE,
				.anchor_point   = SKC::GE::font_options::AP_CENTER
			};
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
		
		if (show_imgui) {
			ImGui::End();
		}
		if (real_imgui_satus != show_imgui) show_imgui = real_imgui_satus;
		window.render(); 
		window.present();
		window.wait_for_frame();
		std::print("frame rate {}{}\r", 1.f / ((float)(window.delta_time()) / 1000),SKC::fmt::clear_line_after);
		++frame;
#pragma endregion
	}
	fl.request_stop(); 
	return 0;
}