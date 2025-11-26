#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "GE/cvars/parse_cvars.hpp"
#include "lua/lua_cxx.hpp" //name WIP
#include "GE/Events.hpp"


using arg_list_t = SKC::GE::C_var_list;
using Lstate_t = SKC::lua::Lua;

struct main_info_t {
    //std::string fname;
    arg_list_t args;
    Lstate_t& lua_state;
    //NOTE(skc): This has to be a reference to preven use after free...
    //(possible bug in lua_close [not checking for the value being null])
};
enum SKC_E {
    sEOK,
    sENO_SDL,
    sENO_WINDOW,
    sENO_LUA,
    sELUA_INVALID,
    sELUA_TYPEMISMATCH,
    sELUA_ENT_NOT_FOUND
};



#ifndef SKC_SDL_INIT_FLAGS
#define SKC_SDL_INIT_FLAGS SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK |SDL_INIT_HAPTIC
#endif // !SKC_SDL_INIT_FLAGS



#define MAIN_NAME proc_main
int MAIN_NAME(main_info_t);

int main(int argc, char** argv) {
    //the goal of this is to abstract away the important (and dirty) stuff like dealing with the CLI
    //arguments

    //this is the lua state
    auto L = Lstate_t();
    arg_list_t args = SKC::GE::parse_c_vars(argc,argv);
    
   
    if (!SDL_Init(SKC_SDL_INIT_FLAGS)) {
        //SDL_Init may fail if it does then we exit...
        auto error = SDL_GetError();
        exit(sENO_SDL);
    }
    //call the user's entry point and save the return value so that we can return it later

    TTF_Init();
    auto ret = MAIN_NAME({args, L});

    SDL_Quit(); //by the time that we get here we are done so we tell sdl to quit.


    //return the return value so that the OS can properly report any errors..
    if (ret != sEOK) {
    }
    return ret;

}
#define main MAIN_NAME
