#pragma once
#include <SDL3/SDL_events.h>
namespace SKC::GE::PRIVATE{
    inline bool should_sikp_event(size_t type){
       // return false; 
        //NOTE(skc) : ignore SDL_EVENT_TEXT_EDITING, SDL_EVENT_TEXT_INPUT and SDL_EVENT_KEYMAP_CHANGED
        //events as they are unused!
        if(type == SDL_EVENT_CLIPBOARD_UPDATE) return true;
        if(type >= SDL_EVENT_RENDER_TARGETS_RESET) return true;
        if(type >= SDL_EVENT_TEXT_EDITING && type < 0x400) return true;
        if (type == SDL_EVENT_MOUSE_ADDED) return true;
        if (type == SDL_EVENT_MOUSE_REMOVED) return true;

        if(type >= 0x200 && type < 0x300 ){
            switch(type){
                case SDL_EVENT_WINDOW_MINIMIZED:
                case SDL_EVENT_WINDOW_MAXIMIZED:
                case SDL_EVENT_WINDOW_MOVED:
                case SDL_EVENT_WINDOW_RESIZED:
                case SDL_EVENT_WINDOW_RESTORED:
                case SDL_EVENT_WINDOW_MOUSE_ENTER:
                case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                case SDL_EVENT_WINDOW_FOCUS_GAINED: {
                    return false;
                }
                default:{return true;}
            }
        }
    return false;
    }
}
