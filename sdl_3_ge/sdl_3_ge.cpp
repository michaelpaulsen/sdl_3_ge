// sdl_3_ge.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>

#include <SDL3/SDL.h>



#include "./lua_cxx.hpp" //name WIP 
#include "./Console.hpp"

//#define NOSDL
//the type of the CLI arguments as passed to the end user 
using arg_list_t = std::vector<std::string>;

using Lstate_t = SKC::lua::Lua; 

struct main_info_t {
    std::string fname; 
    arg_list_t args; 
    Lstate_t lua_state; 
};
constexpr Uint64 TARGET_RENDER_TIME = ((1. / 60.) * 1000); 
enum SKC_E {
    sEOK,
    sENO_SDL, 
    sENO_WINDOW,
    sENO_LUA,
};

#define MAIN_NAME proc_main
#ifndef SKC_SDL_INIT_FLAGS
#define SKC_SDL_INIT_FLAGS SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD
#endif // !SKC_SDL_INIT_FLAGS



int MAIN_NAME(SKC::Console&, main_info_t);

int main(int argc, char **argv){
    //this is going to be moved into its own file... this should not be exposed to the end
    //user's grubby hands  
    //the goal of this is to abstract away the important (and dirty) stuff like dealing with the CLI
    //arguments
    auto console = SKC::Console();
    console.Informln("init skc console");
    //the above is passed on to the user via thier own main function 
    console.Informln("starting LUA VM"); 
    
    //this is the lua state 
    //TODO (skc) : wrap this in an object
     auto L = Lstate_t();
    
    arg_list_t args = {};
    //instead of having a char** this is a std::vector<std::strings> that way the memory deals with its self at the end
    // of exicution this also gives us access to things like args.size() and the such. 
    //argv always at least stores the path to the exe so we only want to enter this loop if the number of paramiters is
    //strictly greater than 1 not greater than or equal 
    if (argc > 1) {
        for (int x = 1; x < argc; ++x) {
            console.Informln("adding ", argv[x], " to args");
            args.emplace_back(argv[x]);
            //add the current paramiter to the stack 
            //should test if this emplace_back is actually good 
        }
        console.Informln("calling user entry with ", args.size(), " arguments");
      //inform the dev on the paramiters... 
    }
    else {
        console.Informln("no paramiters passed args empty"); 
    }
    //we also want to init SDL on our side because there's no reason that you won't want a window 
    console.Informln("init SDL");
#ifndef NOSDL 
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        //SDL_Init may fail if it does then we exit...
        auto error = SDL_GetError(); 
        console.Errorln("UNABLE TO START SDL ERROR ", error);
        console.Errorln("EXITING WITH sENO_SDL");
    }
#else 
    console.Warnln("SDL PRE INIT DISABLED CALL SDL_Init BEFORE USING SDL"); 
#endif
    //call the user's entry point and save the return value so that we can return it later 

    auto ret = MAIN_NAME(console, { argv[0], args, L });
    
    console.Reset(); 
    SDL_Quit(); //by the time that we get here we are done so we tell sdl to quit. 
   
    
    //return the return value so that the OS can properly report any errors..
    if (ret != sEOK) {
        console.Errorln("something whent wrong main funtion returned ", ret, " not ", (int)sEOK);
    }
    return ret; 

}

#define main MAIN_NAME
std::string white_space_to_display(const char t) {
    
    switch (t) {
        case 0x00: return "<NUL>"; 
        case 0x01: return "<SOH>";
        case 0x02: return "<STX>";
        case 0x03: return "<STX>";
        case 0x04: return "<EOT>";
        case 0x05: return "<ENQ>";
        case 0x06: return "<ACK>";
        case 0x07: return "<BEL>";
        case 0x08: return "<BS>";
        case 0x09: return "<TAB>";
        case 0x0a: return "<LF>";
        case 0x0b: return "<VT>";
        case 0x0c: return "<FF>";
        case 0x0d: return "<CR>";
        case 0x0e: return "<SO>";
        case 0x0f: return "<SI>";
        case 0x10: return "<DLE>";
        case 0x11: return "<DC1>";
        case 0x12: return "<DC2>";
        case 0x13: return "<DC3>";
        case 0x14: return "<DC4>";
        case 0x15: return "<NAK>"; 
        case 0x16: return "<SYN>"; 
        case 0x17: return "<ETB>";
        case 0x18: return "<CAN>"; 
        case 0x19: return "<EM>";
        case 0x1a: return "<SUB>"; 
        case 0x1b: return "<ESC>"; 
        case 0x1c: return "<FS>"; 
        case 0x1d: return "<GS>";
        case 0x1e: return "<RS>";
        case 0x1f: return "<US>";
        case 0x20: return "<SPACE>";
        case 0x7f: return "<DEL>";
        default : return std::string(1, t); 
    }
}

void debug_mod_keys(SKC::Console &console, SDL_Keymod mod) {
    if (mod & SDL_KMOD_SHIFT)  { console.Inform("<SHIFT> ");  }
    if (mod & SDL_KMOD_ALT)    { console.Inform("<ALT> ");    }
    if (mod & SDL_KMOD_GUI)    { console.Inform("<MENU> ");   }
    if (mod & SDL_KMOD_SCROLL) { console.Inform("<SCR LCK> ");}
    if (mod & SDL_KMOD_NUM)    { console.Inform("<NUM LCK> ");}
}
void debug_key_codes(SKC::Console& console, int key, bool caps = false) {
    if (key < 256) {
        if (key < 21 || key == 127) {
            console.Inform("key \"", white_space_to_display((char)key), "\"\r");
            return; 
        }
        if (caps) {
            console.Inform("key \"", (char)toupper((char)key), "\"\r");
            return;
        }
        console.Inform("key \"", (char)key, "\"\r");
        return ;
    }
}

void debug_key_events(SKC::Console& console, SDL_Event evnt) {
    auto keye = evnt.key;
    auto key = keye.key;
    auto mod = keye.mod;
    debug_mod_keys(console, mod);
    debug_key_codes(console, key, mod & SDL_KMOD_CAPS);
}

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

            //TODO (skc): make function to calc sleep time 
            //based off a max framerate... 
           
        }
        
        draw_end_ticks = SDL_GetTicks(); 
        console.Inform( "took ", draw_end_ticks - draw_start_tick, "ms (of ", TARGET_RENDER_TIME, ") to draw frame").ClearLine(0).Print('\r');
        
        SDL_Delay(1000);


    }
    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window); 
    return 0; 
}