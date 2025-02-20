// sdl_3_ge.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "main.hpp"
#include "key_util.hpp"

int main(SKC::Console& console, main_info_t info) {
    console.Informln("entered main function");
    SDL_Window* window;
    SDL_Renderer* renderer;
    int window_w = 320, window_h = 240; 
    bool draw = true ;
    if (!SDL_CreateWindowAndRenderer("lib skc test reference window\ntest", window_w, window_h, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        console.Println("SDL error", SDL_GetError());
        return -1;
    }
    SDL_Event evnt{};
    bool quit = false; 
    int spacing = 100; 
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
                    
                    //TODO(skc): make window class 
                    window_w = wind.data1;
                    window_h = wind.data2;
                    break; 
                }
                case SDL_EVENT_WINDOW_FOCUS_LOST: {
                    draw = false; 
                    break; 
                }
                case SDL_EVENT_WINDOW_FOCUS_GAINED: {
                    draw = true; 
                    break; 
                }
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP : {
                    auto keye = evnt.key;
                    auto key = keye.key;
                    auto mod = keye.mod;
                    if (key == SDLK_UP) {
                        if(spacing > 1) --spacing;
                    }
                    
                    if (key == SDLK_DOWN) {
                         ++spacing;

                    }
                    if (key == SDLK_ESCAPE) quit = true;
                    break;
                  
                }
                default: {
                    console.Inform("unknown Event of type : ", evnt.type, '\r').Reset();
                    break; 
                }

            }
            if (quit) break;
        }
        if (draw) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

            //SDL_RenderClear(renderer);
            int x = 0; 
            int y = window_h / 2;
            for (int xx = 0; xx < spacing; ++xx) {
                if (x > window_w) { 
                    y+=5;
                    x -= window_w;
                }
                SDL_SetRenderDrawColor(renderer, x, y-20, 0, 255);
                SDL_RenderPoint(renderer, x, y);
                ++x;
            }
            SDL_RenderPresent(renderer);
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
        if(draw)console.Print("end of frame\r");
    }
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window); 
    return 0; 
}