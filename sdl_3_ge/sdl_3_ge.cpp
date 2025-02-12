// sdl_3_ge.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "main.hpp"
#include "key_util.hpp"
constexpr Uint64 TARGET_RENDER_TIME = ((1. / 60.) * 1000);


int main(SKC::Console& console, main_info_t info) {
    console.Informln("entered main function");
    SDL_Window* window;
    SDL_Renderer* renderer;
    int window_w = 320, window_h = 240; 
    if (!SDL_CreateWindowAndRenderer("lib skc test reference window\ntest", window_w, window_h, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        console.Println("SDL error", SDL_GetError());
        return -1;
    }
    SDL_Event evnt{};
    bool quit = false; 
    while (!quit) {
        Uint64 draw_start_tick = SDL_GetTicks(), draw_end_ticks=0; 
        
        while (SDL_PollEvent(&evnt)) {
            console.ClearLine(); 
            switch (evnt.type) {
                case SDL_EVENT_QUIT: {
                    quit = true; 
                    break; 
                }
                case SDL_EVENT_MOUSE_MOTION: {
                    auto motion = evnt.motion; 
                    console.Inform("mouse ", motion.which, " moved to -> ", motion.x, " ", motion.y, '\r');
                    break; 
                }
                case SDL_EVENT_WINDOW_RESIZED: {
                    auto wind = evnt.window;
                    console.Inform("window resized to ", wind.data1, "x", wind.data2, '\r');
                    break; 
                }
                case SDL_EVENT_WINDOW_FOCUS_LOST :
                case SDL_EVENT_WINDOW_FOCUS_GAINED: {
                    break; 
                }
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP : {
                    debug_key_events(console, evnt); 
                    break;
                  
                }
                default: {
                    console.Inform("unknown Event of type : ", evnt.type, '\r').Reset();
                    break; 
                }

            }
            if (quit) break;
        }
       //FRAME RATE LIMIT CODE!  
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
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window); 
    return 0; 
}