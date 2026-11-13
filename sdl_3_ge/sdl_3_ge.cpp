#define SKC_SDL_INIT_FLAGS  SDL_INIT_VIDEO 
#define NOMINMAX
static constexpr const char   window_size_name[] = "window_size";
static constexpr const char     mouse_pos_name[] = "mouse_pos"  ;
static constexpr const char texture_array_name[] = "textures"   ;
static constexpr const char     draw_loop_name[] = "frame"      ;

#include <print>
#include <string>
#include <thread>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_video.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}

#include "header/commands/command_buffer.hpp"
#include "header/commands/command.hpp"
#include "header/commands/get_key_index.hpp"

#include "./header/commands/init.hpp"
#include "./header/commands/frame.hpp"

#include "header/commands/lua/lua_commands.hpp"
#include "header/commands/lua/lua_commandex.hpp"
#include "header/commands/lua/lua_draw.hpp"
#include "header/commands/lua/lua_render.hpp"
#include "header/commands/lua/lua_tile_texture.hpp"
#include "header/commands/types.hpp"
#include "header/lua/lua.hpp"

#include "header/ini/settings.hpp"

#include "header/GE/Color.hpp"
#include "header/GE/Events.hpp"
#include "header/GE/font.hpp"
#include "header/GE/font_options.hpp"
#include "header/GE/make_uvs.hpp"
#include "header/GE/tile_map.hpp"
#include "header/GE/Window.hpp"
#include "header/math/Vector.hpp"
//#define SKC_NO_BREAK 


#include "header/util.hpp"
#include "header/main.hpp"
#ifdef _DEBUG
#ifdef SKC_NO_BREAK
#define SKC_LE_BREAK(ec)
#elif defined(SKC_EXIT_ON_ERROR)
#define SKC_LE_BREAK(ec) exit(ec)
#elif defined(_WIN32)
#define SKC_LE_BREAK(ec) _CrtDbgBreak()
#elif defined(__clang__)
#define SKC_LE_BREAK(ec) __builtin_trap()
#else 
#define SKC_LE_BREAK(ec) do {\
std::print("[ERROR] unable to detect os at build time please make a ticket with your compiler's"\
"idenifcation macro and debug break intrinsic"\
);
exit(-ec);
}while(0)
#endif
#else
#define SKC_LE_BREAK(ec)
#endif
#define ASERT_COMMAND_VALID(command_state, command)\
	if(skc::is_invalid(command_state)){\
		std::println("INVALID COMMAND {}", command);\
		SKC_LE_BREAK(-EINVAL);\
	}\
	if(command_state == skc::COMMAND_STATE::NOT_IMPLEMENTED){\
		std::println("COMMAND NOT IMPLEMENTED {}", command);\
		SKC_LE_BREAK(-EFAULT);\
	}


static const luaL_Reg skc_stdlib[] = {
   { "command", skc::lua_statics::command_ex    },
   { "render" , skc::lua_statics::render_texture},
   { "clear"  , skc::lua_statics::clear         },
   { "rect"   , skc::lua_statics::draw_rect     },
   { "fill"   , skc::lua_statics::set_draw_color},
   { "tile"   , skc::lua_statics::tile_texture  },
   { "bind"   , skc::lua_statics::bind          },
   { NULL     , NULL                            }
};

int main([[maybe_unused]] main_info_t info) {
//#pragma region set the default values for the program
	try { skc::util::to_number<size_t>("a"); }
	catch (...) {
		std::print("test"); 
	}
	auto  last_update_tick = SDL_GetTicks();
	SKC::Math::Vect2i window_size = {
		1920, 1080
	};
	std::string window_name{ "UNTITLED WINDOW" }, tmessage{};
	SKC::GE::event_handler event_handler{};

	//=========WINDOW SETUP=========
	//
	//TODO(skc) : handle window settings in main.hpp 
	auto window = SKC::GE::window(
		window_name,
		window_size.x,
		window_size.y,
		//SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_TRANSPARENT |
		SDL_WINDOW_HIDDEN
	);

#pragma region font stuff
	auto sc_font = SKC::GE::font("./fonts/FiraCode-Regular.ttf", 64);
	SKC::GE::font_options fo = {
		.x = 20,
		.y = 20,
		.color = SKC::GE::color(255,0,255),
		.line_alignment = SKC::GE::font_options::LINE_ALIGNMENT_CENTER,
		.line_seperator = SKC::GE::font_options::LINE_SEPARATOR_NEWLINE,
		.anchor_point = SKC::GE::font_options::AP_TOP_LEFT
	};
#pragma endregion 
#pragma region pre load stuff
	auto key_binds = skc::commands::types::key_bind_t{};
	window.set_frame_rate(120);
	skc::commands::types::tmap_t txt_names;
	auto lua = skc::lua_engine();
	{
		auto lg = std::lock_guard{ lua };
		lua.load_lib("skc", skc_stdlib);

		lua.create_table();
		lua.append_table("width",  (lua_Number)window_size.x);
		lua.append_table("height", (lua_Number)window_size.y);
		lua.set_global(window_size_name);

		//skc_keys table.
		lua.create_table();
			lua.push("_DEFUALT_TEXTURE");
			lua.create_table();
			lua.append_table("tid", 1LL);
			lua.append_table("width", 16LL);
			lua.append_table("height", 16LL);
			lua.append_table();
		lua.set_global(texture_array_name);
		lua.do_file("./test.lua");
		lua.get_global("init");
		lua.call(0, 0);
	}
	SKC::threading::owning_mutex<Uint64> target_ms_tick{ 50 };
	auto update_thread = std::jthread([&](std::stop_token st) mutable {
		while (!st.stop_requested()) {
			auto now = SDL_GetTicks();
			if (now - last_update_tick > 50) {
				auto lua_lock = std::lock_guard(lua);
				lua.get_global("fixed_update");
				if (lua.is_nil()) return;
				lua.push((double)(SDL_GetTicks() - last_update_tick)/1000.);
				lua.call(1, 0); 
				last_update_tick = SDL_GetTicks(); 

			}
			using namespace std::chrono_literals; 
			std::this_thread::sleep_for(1ms); 
		}
		});
	while (!skc::command_buffer.is_empty()) {
		
		auto line = skc::command_buffer.pop_front();
		auto command = skc::command(line);
		auto cmd = command.cmd();
		auto c_s = skc::commands::parse_init(lua, window, command, txt_names, window_size);
		ASERT_COMMAND_VALID(c_s, command.str());
		if (skc::has_parsed_command(c_s)) continue; 
		c_s = skc::commands::parse_frame(lua, window, command, key_binds,txt_names); 
		if (skc::should_exit(c_s)) std::exit(0);
		ASERT_COMMAND_VALID(c_s, command.str());
	}
	window.set_render_target(0);
	window.show();
#pragma endregion
	//=========MAIN LOOP START =========
	while (true) {
		event_handler.pollevents();
		[[maybe_unused]] Uint64 frame = window.frame();
		[[maybe_unused]] float  fframe = (float)frame;
		[[maybe_unused]] auto   mpos = event_handler.cursor_position();
		if (event_handler.window_resized()) {
			auto lg = std::lock_guard(lua);
			window_size = window.get_dimentions();
			lua.get_global(window_size_name); 
			lua.append_table("width", (lua_Number)window_size.x);
			lua.append_table("height", (lua_Number)window_size.y);
			lua.set_global(window_size_name);
		}

		if (event_handler.quit()) break;
		auto mousep = event_handler.mouse_position(); 
		{
			auto lg = std::lock_guard(lua);
			lua.create_table();
			lua.append_table("x", mousep.x);
			lua.append_table("y", mousep.y);
			lua.set_global(mouse_pos_name);
			lua.get_global(draw_loop_name);
			lua.push((long long)frame);
			lua.call(1, 0);
			if (lua.errored()) {
				return 1;
			}
		}
		//TODO(skc) : move this to its own file 
		namespace skcGE = SKC::GE; 
		auto lmb = event_handler.get_mouse_button_state(event_handler.LEFT_MOUSE_BUTTON);
		if (lmb.down) {
			const auto leftdownid = UZ(SKC::GE::types::E_EVENT_IDS::MOUSE_LEFT_DOWN); 
			if (key_binds.contains(leftdownid)) {
				auto function = key_binds.at(leftdownid);
				if (function != "nil") {
					auto lg = std::lock_guard(lua);
					lua.get_global(function);
					if (!lua.is_nil()) {
						lua.push((int64_t)event_handler.LEFT_MOUSE_BUTTON);
						lua.push((long long)lmb.clicks);
						lua.push((long long)lmb.down);
						lua.push((long long)lmb.x);
						lua.push((long long)lmb.y);
						lua.push((long long)event_handler.relative_mouse_position().x);
						lua.push((long long)event_handler.relative_mouse_position().y);
						lua.call(7, 0);
					}
				}
			}
		}
		auto rmb = event_handler.get_mouse_button_state(event_handler.RIGHT_MOUSE_BUTTON);
		if (rmb.down) {
			const auto leftdownid = UZ(SKC::GE::types::E_EVENT_IDS::MOUSE_RIGHT_DOWN);
			if (key_binds.contains(leftdownid)) {
				auto function = key_binds.at(leftdownid);
				if (function != "nil") {
					auto lg = std::lock_guard(lua);
					lua.get_global(function);
					if (!lua.is_nil()) {
						lua.push((int64_t)event_handler.RIGHT_MOUSE_BUTTON);
						lua.push((long long)rmb.clicks);
						lua.push((long long)rmb.down);
						lua.push((long long)rmb.x);
						lua.push((long long)rmb.y);
						lua.push((long long)event_handler.relative_mouse_position().x);
						lua.push((long long)event_handler.relative_mouse_position().y);
						lua.call(7, 0);
					}
				}
			}
		}
		for (const auto& [key, function] : key_binds) {
			if (key >= std::to_underlying<skcGE::types::special_keys>(skcGE::types::special_keys::LAST) || function == "nil"){continue;}
			auto key_state = event_handler.get_key_state(key); 
			if (key_state.down) {
				auto lg = std::lock_guard(lua);
				lua.get_global(function);
				lua.push((lua_Integer)key);
				lua.push(key_state.pressed);
				lua.push((lua_Number)key_state.delta / 1e9);
				lua.push((lua_Number)key_state.last  / 1e9);
				lua.call(4, 0); 
			}
		}

		while (!skc::command_buffer.is_empty()) {
			auto line = skc::command_buffer.pop_front();
			auto command = skc::command(line);
			auto cmd = command.cmd();
			auto c_s = skc::commands::parse_frame(lua, window, command, key_binds, txt_names);
			if (skc::should_exit(c_s)) std::exit(0);
			ASERT_COMMAND_VALID(c_s, command.str());
		}
		
		window.present();
		window.wait_for_frame();
	}
	return 0;
}