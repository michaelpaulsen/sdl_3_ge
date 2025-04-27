#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <ranges>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>


#include "./key_util.hpp"
#include "./game_pad.hpp"
#include "./CVar.hpp"
#include "./lua_cxx.hpp" //name WIP 
#include "./Console.hpp"
#include "./Events.hpp"
#include "./Window.hpp"
#include "./Imgui_window.hpp"

//TODO(skc): shouldn't be const? 
constexpr Uint64 TARGET_RENDER_TIME = static_cast<Uint64>((1. / 30.) * 1000);



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

#define MAIN_NAME proc_main
#ifndef SKC_SDL_INIT_FLAGS
#define SKC_SDL_INIT_FLAGS SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK |SDL_INIT_HAPTIC 
#endif // !SKC_SDL_INIT_FLAGS



int MAIN_NAME(SKC::Console&, main_info_t);

int main(int argc, char** argv) {
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
    //instead of having a char** this is a list of the C_vars  that way the memory deals with its self at the end
    // of exicution this also gives us access to things like args.size() and the such. 
    console.Informln("adding ", argv[0], " to args as \"exe_path\"");
    args.emplace_back("exe_path", std::string(argv[0])); 
    if (argc > 1) {
        namespace rng = std::ranges; 
        for (int x = 1; x < argc; ++x) {
            std::string arg = argv[x]; 
            if (!arg.contains('=')) {
                
                console.Inform("adding ", arg, " to Cvars as <EMPTY>");
                args.emplace_back(arg);
                continue; 
            }
            auto arg_rng = rng::split_view(arg, '=');
            bool is_name{ true }; 
            std::string name;
            std::string value;
            for (const auto& ev : arg_rng) {
                std::string_view data{ ev }; 
                if (is_name) {
                    name = data; 
                    is_name = false; 
                    continue; 
                }
                value += data; 

            }
            if (!name.empty()) {
                if (value.empty()) {
                    console.Informln ("adding ", name, " to Cvars as <EMPTY>");
                    args.emplace_back(name); 
                    continue;
                }
                console.Informln("adding ", name, " to Cvars as ", value);
                args.emplace_back(name, value);

            }
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
    auto ret = MAIN_NAME(console, {args, L});
   
    console.Reset();
    SDL_Quit(); //by the time that we get here we are done so we tell sdl to quit. 


    //return the return value so that the OS can properly report any errors..
    if (ret != sEOK) {
        console.Errorln("something whent wrong main funtion returned ", ret, " not ", (int)sEOK);
    }
    return ret;

}
#define main MAIN_NAME
