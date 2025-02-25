// sdl_3_ge.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "main.hpp"
#define EVENT_LAMBDA  [&console](SDL_Event e) mutable -> SKC::GE::event_t

int main(SKC::Console& console, main_info_t info) {
    console.Informln("entered main function");
    int window_w = 320, window_h = 240;
    bool draw = true;
    auto window = SKC::GE::window("test window", window_w, window_h, SDL_WINDOW_RESIZABLE); 
    SDL_Event evnt{};
    bool quit = false;
    int spacing = 100;
    auto event_handler = SKC::GE::event_handler(); 
    
    //default key event
    auto keyevent = EVENT_LAMBDA {
        auto key_event = e.key;
        auto key = key_event.key;
        console.Print("key", (int)key, " was ", key_event.down ? " pressed" : " released", '\r');
        return SKC::GE::event_t::CONTINUE; 
    
    }; 
    auto mouse_event = EVENT_LAMBDA{
        auto motion = e.motion;
        console.Inform("mouse ", motion.which, " moved to -> ", motion.x, " ", motion.y, '\r');
        return SKC::GE::event_t::CONTINUE;
    };
    event_handler.register_event(SDL_EVENT_KEY_UP, keyevent);
    event_handler.register_event(SDL_EVENT_KEY_DOWN, keyevent);
    event_handler.register_event(SDL_EVENT_MOUSE_MOTION, mouse_event);
    event_handler.register_event(SDL_EVENT_MOUSE_MOTION, EVENT_LAMBDA{
        auto jbat = e.jbattery; 
            return SKC::GE::event_t::CONTINUE; 
        });
    event_handler.register_event(SDL_EVENT_WINDOW_RESIZED, [&console, &window](SDL_Event e) {
        return SKC::GE::event_t::CONTINUE;
        });
    event_handler.register_event(SDL_EVENT_MOUSE_WHEEL, EVENT_LAMBDA{
        auto wheel = e.wheel; 
    console.Print("\rmouse wheel event whith ", wheel.x, 'x', wheel.y, "and mouse pos of ", wheel.mouse_x, 'x', wheel.mouse_y,'\r');
        return SKC::GE::event_t::CONTINUE; 
    });
    while (!quit) {
        Uint64 draw_start_tick = SDL_GetTicks(), draw_end_ticks = 0;

        while (SDL_PollEvent(&evnt)) {
            console.ClearLine();
            auto evnd = event_handler.do_event(evnt);
            if (evnd == SKC::GE::event_t::QUIT) {

                quit = true;
                break;
            }
            if (evnd == SKC::GE::event_t::NO_FUNCT) {

                //TODO(skc):remove all of this code for the event API... 
                switch (evnt.type) {

                    //TODO(skc): use event handler instead of this.
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
                    //the event
                    auto gpevent = evnt.gbutton;
                    //the controller that the event is based in ...
                    auto a = gpevent.button;
                    auto contr = event_handler.get_game_pad(gpevent.which);
                    if (contr.has_value()) {
                        auto t = SKC::GE::controller::get_controller_face_button_name(contr.value(), gpevent.button);
                        if (gpevent.down && t.has_value()) {
                            console.ClearLine().Print("\rbtn \"", t.value(), "\" pressed");
                        }
                    }
                    else {
                        console.Print(contr.error());
                    }
                    break;
                }
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
                    std::cout << "unhandled event of type 0x" << std::hex << evnt.type << std::dec << '\r';
                    break;
                }
                }
                if (draw) {
                    window.clear();
                    window.set_draw_color(255, 0, 0);
                    window.fill_rectangle(0, 0, 20, 20);
                    window.set_draw_color(255, 255, 0);
                    window.draw_rectangle({ 1,1,20,50 }); 
                    window.present();
                }
                if (quit) break;
            }
        }
        //FRAME RATE LIMIT CODE! DO ALL DRAWING BEFORE THIS LINE   
        //(^ is here for searchablility DO NOT REMOVE) 
        draw_end_ticks = SDL_GetTicks();
        //this is the current tick since the SDL_Timer modual was started
        auto rt = draw_end_ticks - draw_start_tick;
        //this is the number of ticks that has passed since the start of the draw code
        if (rt > TARGET_RENDER_TIME) rt = 0;
        //rt is unsigned so we need to check if it is greater than the time we want to spend 
        //rendering if so we set it to 0 
        rt = TARGET_RENDER_TIME - rt;
        //we do not need rt after this point so we can just write to it .. 
        // (saves memory or something?) 
        SDL_Delay(rt);
        //delay the main thread for rt (which now has the wait time )ms 
        
    }
    return 0;
}