// sdl_3_ge.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define EVENT_LAMBDA  [&console](SDL_Event e) mutable -> SKC::GE::event_t

#include "main.hpp"
#include "./DBG_functions.h"

#include <cmath> 
#include <vector>
#include <ctime>

static constexpr size_t MAX_INPUT_LEN = 200; 
int  to_seconds(int h, int m, int s) {
    return (h * 3600) + (m * 60) + s; 
}
std::string calc_time_remaining(time_t now, int start_h, int start_m, int start_s, bool loop = false) {
    //std::string out{}; 
    tm local_tm{};
    localtime_s(&local_tm, &now);
    int seconds_since_start_of_day = to_seconds(local_tm.tm_hour, local_tm.tm_min, local_tm.tm_sec);
    
    long long target_time = to_seconds(start_h, start_m, start_s);
    long long difference = target_time - seconds_since_start_of_day;
    bool count_up = false; 
    if (difference < 0) {
        if (!loop) return "Starting Soon";
        difference = seconds_since_start_of_day - target_time;
        count_up = true; 
    }
    //while (difference < 0) difference += (24ll * 3600);
    int hour{};
    while (difference > 3600) {
        difference -= 3600; 
        ++hour; 
    }
    int min{}; 
    while (difference > 60) {
        difference -= 60;
        ++min;
    }
    return std::format("Starting in T{}{:0>2}:{:0>2}:{:0>2}",count_up?'+' : '-',hour, min, difference);

}

namespace fs = std::filesystem;

int main(SKC::Console& console, main_info_t info) {
    console.Informln("entered main function");
    std::string data = std::string(MAX_INPUT_LEN, 0);
 

    float mouse_x{}, mouse_y{}, size{ 1440 }, text_scale{ 0.650f },  rot{}, rot_speed{ -1 };
    int window_h{1080}, window_w{1920}, f{},
        counter{}, advatage{},
        die_roll{}, die_roll1{}, die_roll2{},
        start_h{ 9 }, start_m{ 30 }, start_s{ 0 };
    std::vector<int> dice{2,4,6,8,10,20,100}; 
    bool loop_timer{ false }, show_debug_window{ true };
    Uint64 frame{};
    //TODO(skc) : handle window setting in main.hpp 
    if (info.args.has("h")) {
        auto cvar = info.args.get_as<int>("h");
        window_h = cvar.value(); 
    }
     if (info.args.has("w")) {
         auto cvar = info.args.get_as<int>("w");
         window_w = cvar.value();
     }
    
    auto window = SKC::GE::imgui_window("test window", window_w, window_h, SDL_WINDOW_RESIZABLE);
    auto window_name = window.get_window_title(); 
    SDL_Event evnt{};
    bool quit = false;
    auto event_handler = SKC::GE::event_handler();
    float x = 59.f;
    float y = 14.f;
    ImVec4 text_color{0.71f,0.082f,0.082f,1.f};
    
    
    
    //default key event
    Uint64 last_frame_time{0};
    
    //TODO(skc): implement a function that does this automatically
    SDL_Surface *surface = SDL_LoadBMP("./test_image.bmp");
    if (!surface) {
        console.Errorln("SDL_ERROR ", SDL_GetError());
        exit(-1);
    }
    //FONT STUFF 
    //TODO (skc) : create a resource class. 
    auto font_dir = fs::path("C:\\Windows\\Fonts");
    auto font_path = font_dir / "calibrib.ttf";


    if (!fs::directory_entry(font_path).exists()) {
        return 0;
    }
  
    auto font = TTF_OpenFont(font_path.generic_string().c_str(), 64);
    auto font_big = TTF_OpenFont(font_path.generic_string().c_str(), 200);

    auto keyevent = [&size, &show_debug_window, &console](SDL_Event e) mutable {
        auto key_event = e.key;
        auto key = key_event.key;
        if (e.type == SDL_EVENT_KEY_UP) {
            if (key == SDLK_ESCAPE) {
                show_debug_window = !show_debug_window;
                return SKC::GE::event_t::CONTINUE;

            }
            if (key == SDLK_PLUS) {
                size += 10; 
            }
            if (key == SDLK_MINUS) {
                size -= 10;
            }
        }
        return SKC::GE::event_t::CONTINUE;

        };
    auto mouse_event = EVENT_LAMBDA{
        auto motion = e.motion;
        console.Inform("mouse ", motion.which, " moved to -> ", motion.x, " ", motion.y, '\r');
        return SKC::GE::event_t::CONTINUE;
    };

    auto test_texture = window.create_texture_from_surface(surface); 
    SDL_DestroySurface(surface); 
    window.set_background_color({ 0, 0, 0,0 });
    
    event_handler.register_event(SDL_EVENT_KEY_UP, keyevent);
    event_handler.register_event(SDL_EVENT_KEY_DOWN, keyevent);
    event_handler.register_event(SDL_EVENT_MOUSE_MOTION, 
        [&console, &mouse_x, &mouse_y](SDL_Event e) mutable -> SKC::GE::event_t {
            mouse_x = e.motion.x;
            mouse_y = e.motion.y;
            return SKC::GE::event_t::CONTINUE; 
        });
    event_handler.register_event(SDL_EVENT_WINDOW_RESIZED, [&console, &window, &window_h, &window_w](SDL_Event e) {
        window.update_window_size();
        window.get_window_dimentions(window_w, window_h); 
        return SKC::GE::event_t::CONTINUE;
    });
   
    
    event_handler.register_event(SDL_EVENT_MOUSE_WHEEL, [&console, &window, &rot_speed](SDL_Event e) {
        auto wheel = e.wheel; 
        console.Print("\rmouse wheel event whith ", wheel.x, 'x', wheel.y, "and mouse pos of ", wheel.mouse_x, 'x', wheel.mouse_y,'\r');
        rot_speed += wheel.y; 
        return SKC::GE::event_t::CONTINUE; 
    });
   
    while (!quit) {
        Uint64 draw_start_tick = SDL_GetTicks(), draw_end_ticks = 0;
    
        while (SDL_PollEvent(&evnt)) {
            console.ClearLine();
            auto evnd = event_handler.do_event(evnt);
            ImGui_ImplSDL3_ProcessEvent(&evnt);
            
            if (evnd == SKC::GE::event_t::QUIT) {
                quit = true;
                break;
            }
            if (evnd == SKC::GE::event_t::NO_FUNCT) {

                //TODO(skc):remove all of this code for the event API... 
                switch (evnt.type) {

                case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
                    auto gp_axis_motion = evnt.gaxis;
                    int gp_axis = gp_axis_motion.axis;
                    int value = gp_axis_motion.value;
                    double r = (double)value / (double)32767;
                    console.ClearLine().Print("\raxis ", gp_axis, " is value ", r);
                    break;
                }
                case SDL_EVENT_GAMEPAD_REMAPPED: {
                    break;
                }
                default: {
                    auto type = evnt.type;
                    if (type <= SDL_EVENT_WINDOW_LAST && type >= SDL_EVENT_WINDOW_FIRST) {
                        console.ClearLine().Print("unhandled window event\r");
                        break;
                    }
                    std::cout << std::format("unhandled event of type 0x{:04x}\r", evnt.type);
                    break;
                }
                }

            }

        }

        
        
        window.start_frame(); 
        if (show_debug_window) {

            ImGui::Begin("options menue!", &show_debug_window);                          // Create a window called "Hello, world!" and append into it.
            
            ImGui::SeparatorText("Text Stuff");
            
            //ImGui::Text("last frame time %d ms (%0.2f fps) ", last_frame_time, 1000.f/(float)last_frame_time);
            ImGui::InputText("test", data.data(), MAX_INPUT_LEN+1);
            ImGui::SliderFloat("text X pos", &x, 0.f, (float)window_w);
            ImGui::SliderFloat ("text y pos", &y,0.f, (float)window_h);
            ImGui::DragFloat("text scale factor", &text_scale,.05f,0.01f,0.f);
            ImGui::ColorEdit3("text color", (float*)&text_color);
            //text_color
            ImGui::SeparatorText("Timer Stuff");
            ImGui::Checkbox("loop timer", &loop_timer);
            ImGui::SliderInt("Start Hour", &start_h, 0, 24);
            ImGui::SliderInt("Start Minute", &start_m, 0, 59);
            ImGui::SliderInt("Start Second", &start_s, 0, 59);

            ImGui::SeparatorText("Rotation Stuff");
            ImGui::SliderFloat("rotation ", &rot, 0.f, 360.f, "%.0f deg");
            ImGui::SliderFloat("rotation unwind speed", &rot_speed, -360.f, 360.f, "%.0f deg/frame");
            ImGui::DragFloat("face zoom",&size);
           
            
            ImGui::End();

        }
        window.render();
        window.clear();
        float aspect = (float)test_texture->w / (float)test_texture->h;
        SKC::GE::Frect pos = { 
            (float)window.from_normilzed_width(.5) - (size * aspect) / 2,
            (float)window.from_normilzed_height(.5) - size/2, size* aspect , size};
        //#B51515;
        window.draw_texture_rotated(test_texture,pos, rot);
        window.set_window_render_scale(text_scale);
#pragma warning(push)
#pragma warning( disable : 4244)
        window.render_text_simple(std::format("{}", data), font_big, x / text_scale, y / text_scale, text_color.x * 255, text_color.y * 255, text_color.z * 255);
        auto now = time(0);
        tm time_stamp{}; 
        localtime_s(&time_stamp, &now); 


        window.render_text_simple(
            calc_time_remaining(now, start_h, start_m, start_s, true), font_big,
            x / text_scale, (y / text_scale)+ 256/text_scale,
            text_color.x * 255, text_color.y * 255, text_color.z * 255);
        
#pragma warning(pop)
        if (frame >= 60) {
            if ((frame % 60) < 30) {
                window.set_window_title(std::format("{} ({} frame : {})", window_name, info.args.get_as<std::string>("exe_path").value(),frame%60));
            }
            else {
                window.set_window_title(std::format("{} (frame:{})", window_name, frame%60));
            }
        }
        window.set_window_render_scale(1);
        auto str = std::format("{:0>2}:{:0>2}:{:0>2}", time_stamp.tm_hour, time_stamp.tm_min, time_stamp.tm_sec); 
        auto tmp_y = (float)window.from_normilzed_height(.90); 
        window.render_text_simple(str,
            font, 16, tmp_y,0,0,0,200);
        window.draw_imgui_data();
        window.present();
        rot += rot_speed;
        while(rot < 0) rot = rot+360;
        while(rot > 360) rot -= 360;
        
        
        if (quit) break;
        
        //FRAME RATE LIMIT CODE! DO ALL DRAWING BEFORE THIS LINE   
        //(^ is here for searchablility DO NOT REMOVE) 
        draw_end_ticks = SDL_GetTicks();
        //this is the current tick since the SDL_Timer modual was started
        auto rt = draw_end_ticks - draw_start_tick;
        Uint32 wait_time = static_cast<Uint32>(TARGET_RENDER_TIME) - static_cast<Uint32>(rt);
        //this is the number of ticks that has passed since the start of the draw code
        if (rt> TARGET_RENDER_TIME) wait_time = 0;
        //rt is unsigned so we need to check if it is greater than the time we want to spend 
        //rendering if so we set it to 0 
        SDL_Delay(wait_time);
        if (rt < TARGET_RENDER_TIME) {
            last_frame_time = TARGET_RENDER_TIME;
        }
        else {
            last_frame_time = rt;

        }

        ++frame; 
    }
    return 0;
}