#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "./key_util.hpp"
#include "./game_pad.hpp"

#include "./lua_cxx.hpp" //name WIP 
#include "./Console.hpp"
#include "./Events.hpp"
#include "./Window.hpp"
//TODO(skc): shouldn't be const? 
constexpr Uint64 TARGET_RENDER_TIME = ((1. / 60.) * 1000);



//#define NOSDL
//the type of the CLI arguments as passed to the end user 
using arg_list_t = std::vector<std::string>;

using Lstate_t = SKC::lua::Lua;

struct main_info_t {
    std::string fname;
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

#define MAIN_NAME proc_main
#ifndef SKC_SDL_INIT_FLAGS
#define SKC_SDL_INIT_FLAGS SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK |SDL_INIT_HAPTIC 
#endif // !SKC_SDL_INIT_FLAGS



int MAIN_NAME(SKC::Console&, main_info_t);

int main(int argc, char** argv) {
    //user's grubby hands  
    //the goal of this is to abstract away the important (and dirty) stuff like dealing with the CLI
    //arguments
    auto console = SKC::Console();
    console.Clear().Informln("init skc console");
    //the above is passed on to the user via thier own main function 
    console.Informln("starting LUA VM");

    //this is the lua state 
    auto L = Lstate_t();
    if (!L.execute_command()) {
        console.Error("LUA TEST FAILD");
    }
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
    console.Informln("init SDL");
    if (!SDL_Init(SKC_SDL_INIT_FLAGS)) {
        //SDL_Init may fail if it does then we exit...
        auto error = SDL_GetError();
        console.Errorln("UNABLE TO START SDL ERROR ", error);
        console.Errorln("EXITING WITH sENO_SDL");
        exit(sENO_SDL); 
    }
    //call the user's entry point and save the return value so that we can return it later 
    TTF_Init(); 
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
