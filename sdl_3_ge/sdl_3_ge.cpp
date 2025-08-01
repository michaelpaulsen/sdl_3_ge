// sdl_3_ge.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <cmath> 
#include <vector>
#include <ctime>
#include <fstream>
#include <format>
#include <print>
#include <thread>


#include "header/Vector.hpp"
#include "header/main.hpp"
#include "header/DBG_functions.h"

constexpr size_t MESSAGE_SIZE = 255;
void increment_minutes(int& hour, int& minute, int increment = 1) {
	minute += increment;
	if (minute >= 60) {
		minute -= 60;
		++hour; 
		if (hour > 24) {
			hour -= 24;
		}
	}
	if(minute < 0) {
		minute += 60;
		--hour; 
		if (hour < 0) {
			hour += 24;
		}
	}
}
void increment_seconds(int& hour, int& minute, int& second, int increment = 1) {
	second += increment;
	if (second >= 60) {
		second -= 60;
		increment_minutes(hour, minute);
	}
	if(second < 0) {
		second += 60;
		increment_minutes(hour, minute, -1);

	}
}
void increment_time(int& hour, int& minute, int& second, int mode, bool inc, int increment = 1) {
	
	if (inc) {
		
		if (mode == 0) {
			hour = SKC::Math::wrap(hour + increment, 0, 23);
		}
		if (mode == 1) {
			increment_minutes(hour, minute, increment);
		}	
		if (mode == 2) {
			increment_seconds(hour, minute, second, increment);
		}
	}
}

auto now_as_time_t() {
	auto current_time = std::chrono::system_clock::now();
	auto timet = std::chrono::system_clock::to_time_t(current_time);
	tm time_struct;
	localtime_s(&time_struct, &timet);
	return time_struct; 
}
long long get_seconds(int hour, int minute, int second) {
	//convert the time to seconds since the start of the day
	return (hour * 60 + minute) * 60 + second;
}
long long get_current_time() {
	//get the current time in seconds since the start of the day
	tm time_struct =  now_as_time_t();
	return get_seconds(time_struct.tm_hour, time_struct.tm_min, time_struct.tm_sec);
}
std::string time_until(int hour, int minute, int second) {
	auto current_time = get_current_time(); 
	auto target_time = get_seconds(hour, minute, second);
	if (target_time < current_time)  return "starting soon";
	auto time_diff = target_time - current_time;
	return std::format("{:02}:{:02}:{:02}", time_diff / 3600, (time_diff / 60) % 60, time_diff % 60);
}

int main(SKC::Console& console, main_info_t info) {
	int text_alpha = 255; 
	int hour = info.args.get_as("hour", 9), //default 9 am
	 minute = info.args.get_as("minute", 30), //default 30 minutes
	 second = info.args.get_as("second", 0); //default 0 seconds
	//TODO(skc) : handle window settings in main.hpp 
	int window_h{ info.args.get_as("h",1080) }, window_w{ info.args.get_as("w",1920) };
	bool do_render{ info.args.get_as("render", true) };
	bool degug_mode{ info.args.get_as("debug", false) };
	if (!do_render) return 0; 
	
	uint64_t last_frame_time{}; 
	Uint64 frame{};
	//auto r1 = SKC::GE::room("test",65,65); 
	double draw_time = 0, fframe = 0; 

	auto window = SKC::GE::imgui_window("SKELETON GAME ENGINE TEST APPLICATION",
		window_w, window_h,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT);
	
	//auto player_texture = window.create_texture_from_path("./player Icon.bmp"); 
	//hint to sdl that I want it to listen for events even when the window is not focused
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
	int x = 0, y = 0;
	SKC::GE::event_handler<true> event_handler{};



	
	//TODO(skc) : make this a function in the window class
	int font_size = 24 * 4;
	auto font = TTF_OpenFont("./fonts/arial.ttf", font_size);
//=========MAIN LOOP START =========
	bool inc = true;
	auto message = std::string(MESSAGE_SIZE, char(0));
	window.set_background_color(0,0,0,0);
	while (true) {
		bool rendering_text = false;
		Uint64 draw_start_tick = SDL_GetTicks(), draw_end_ticks = 0;
		//window.set_render_scale(2); 
		window.clear();
		event_handler.pollevents(); 
		auto keymod_state = event_handler.get_key_mods(); 
		if (event_handler.quit()) break; 
		
		
		bool window_resized = event_handler.window_resized();
		if (window_resized) {
			window.update_window_size();
		}
#pragma region set the mode
		int mode = 2; 
		if (event_handler.get_key_mods().shift()) mode = 0;
		if (event_handler.get_key_mods().ctrl()) mode = 1;
#pragma endregion
#pragma region render text
		auto x_pos = window.from_normilzed_width(.5) + x;
		auto y_pos = window.from_normilzed_height(.5) + y;
		if (get_current_time() > get_seconds(hour, minute, second)) {
			text_alpha -= 2;
		}
		else if (text_alpha != 255) {
			text_alpha += 8;
		}
		if (text_alpha <   0) text_alpha =   0;
		if (text_alpha > 255) text_alpha = 255;

		if (text_alpha > 0) {
			rendering_text = true; 
			window.render_text_centered_simple(std::format("starting at {}:{:02}:{:02}", hour, minute, second),
				font,
				x_pos - 20,
				y_pos - (font_size*1.5),
				255, 255, 255, text_alpha
			);
			window.render_text_centered_simple(
				time_until(hour, minute, second),
				font, 
				x_pos,
				y_pos,
				255, 255, 255, text_alpha
			);
		}
		
#pragma endregion
#pragma region accept user input 
		//if(event_handler.)
		constexpr int INC_FRAME = 16; 
		auto l_key_state = event_handler.get_arrow_key_state(SKC::GE::arrow_direction_t::RIGHT), 
			j_key_state = event_handler.get_arrow_key_state(SKC::GE::arrow_direction_t::LEFT) ;
		if (event_handler.scroll_wheel_y() != 0) {
			if (event_handler.get_key_mods().shift()) {
				if (event_handler.get_key_mods().alt()) {
					y += event_handler.scroll_wheel_y() * 4; //move text up and down
				}
				else {
					x += event_handler.scroll_wheel_y() * 4; //move text left and righ
				}
			}
			else {
				font_size += event_handler.scroll_wheel_y(); 
				TTF_CloseFont(font);
				font = TTF_OpenFont("./fonts/arial.ttf", font_size);

			}
			
			
		}
		if (event_handler.get_key_state('`').down) {
			if (inc) {
				degug_mode = !degug_mode;
			}
			inc = false;
		} 
		else if (event_handler.get_key_state('n').down && event_handler.get_key_mods().ctrl()) {
			tm time_struct = now_as_time_t();
			hour = time_struct.tm_hour;
			minute = time_struct.tm_min;
			second = time_struct.tm_sec - 1;
			inc = false;
		}
		else if (l_key_state.down) {
			increment_time(hour, minute, second, mode, inc);
				
			inc = !(frame% INC_FRAME);
		}
		else if (j_key_state.down) {
			increment_time(hour, minute, second, mode, inc, -1);
			inc = !(frame % INC_FRAME);
		}
		else {
			inc = true;
		}
#pragma endregion
		//=====imgui CODE ====
#pragma region imgui code
		window.set_render_scale(1);

		window.start_frame();
		

		if(degug_mode) { 
			ImGui::Begin("DBG window 1", &degug_mode);
			if (frame) {
			ImGui::Text(std::format("last frame time (ms) : {}", last_frame_time).c_str());
			ImGui::Text(std::format("average frame time (ms) : {:0.3} ({:0.6} fps)", (draw_time/fframe), 1000/(draw_time / fframe)).c_str());
			ImGui::Text(std::format("FPS : {}", 1000.f / last_frame_time).c_str());
			ImGui::Text(std::format("target FPS : {}", 1000.f/TARGET_RENDER_TIME).c_str());
			}
			ImGui::Checkbox("rendering the text", &rendering_text); 
			 
			ImGui::Text(std::format("font size : {}", font_size).c_str());
			ImGui::Text(std::format("text pos: {}x{}", x,y).c_str());
			ImGui::End();
		
		
			ImGui::Begin("DBG settings window");
			ImGui::Checkbox("window resized event", &window_resized);
			ImGui::Checkbox("inc", &inc);
			ImGui::InputText("message", message.data(), MESSAGE_SIZE);
			ImGui::SliderInt("start hour", &hour, 0, 23);
			ImGui::SliderInt("start minute", &minute, 0, 59);
			ImGui::SliderInt("start second", &second, 0, 59);
			ImGui::End();
		}
		window.render(); 
		window.draw_imgui_data();
#pragma endregion
		window.present();
		//get the number of threads that are running in the program
		
		
		//=== FRAME RATE LIMIT CODE! ===  
		//DO ALL DRAWING BEFORE ABOVE LINE   
		//(^ is here for searchablility DO NOT REMOVE) 
		//TODO(skc) This should be a function in the window class 
		//named something like window::wait_for_next_frame(uint64_t &render_time);
		draw_end_ticks = SDL_GetTicks();
		//this is the current tick since the SDL_Timer modual was started
		auto rt = draw_end_ticks - draw_start_tick;
		Uint32 wait_time = static_cast<Uint32>(TARGET_RENDER_TIME) - static_cast<Uint32>(rt);
		//this is the number of ticks that has passed since the start of the draw code
		if (rt > TARGET_RENDER_TIME) wait_time = 0;
		//rt is unsigned so we need to check if it is greater than the time we want to spend 
		//rendering if so we set it to 0 
		SDL_Delay(wait_time);
		fframe++;
		if (rt < TARGET_RENDER_TIME) {
			last_frame_time = TARGET_RENDER_TIME;
		}
		else {
			last_frame_time = rt;
		}
		draw_time += last_frame_time;
		++frame;
		

	}
	
	return 0;
}