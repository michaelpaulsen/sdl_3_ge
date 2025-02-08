// sdl_3_ge.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "main.hpp"

constexpr Uint64 TARGET_RENDER_TIME = ((1. / 60.) * 1000);

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