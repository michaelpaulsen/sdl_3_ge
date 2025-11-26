#pragma once
#include <format>

#pragma warning( push ) // Save the current warning state
#pragma warning( disable : 4100 ) // Disable warning C4996 (e.g., for deprecated functions like `scanf`)

// Your code that generates warning C4996

namespace SKC::fmt {
		//NOTE(skc): 
		// SA https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
		enum class move_dir_t {
			UP, DOWN, LEFT, RIGHT
		};
		//TODO(skc): find better names for these..
		struct ES {} es;
		
		struct fmt_go_home {};
		struct fmt_move {
			int lines = { 1 };
			move_dir_t dir{ move_dir_t::UP };

		};
		struct fmt_clear_screen {
			int mode;
		};
		struct fmt_clear_line {
			int mode{ 0 }; 
		};
		struct fmt_style_text {
			int mode; 
		};
		//TODO(skc): 
		//ESC[#E	moves cursor to beginning of next line, # lines down
		//ESC[#F	moves cursor to beginning of previous line, # lines up
		struct fmt_set_palleted_color {
			int mode{}, fg_color{}, bg_color{};
		};

		struct fmt_save_loc {}; 
		struct fmt_restore_loc {};
		//NOTE(skc) : ANSI specifies that 1,1 is the top left of the screen.. not 0,0
		struct fmt_jump_to {
			int line{ 1 }, col{ 1 };
		};
		//TODO(skc)
		// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#256-colors
	
	const fmt::fmt_go_home      home{}; 
	const fmt::fmt_save_loc     save_cursor_position{};
	const fmt::fmt_restore_loc  restore_cursor_position{};

	const fmt::fmt_clear_screen clear_remaining_screen{};
	const fmt::fmt_clear_screen clear_screen_above_cursor{ 1 };
	const fmt::fmt_clear_screen clear_whole_screen{ 2 };
	const fmt::fmt_clear_screen clear_saved_line_buffer{ 3 };
	const fmt::fmt_clear_line   clear_line_after{};
	const fmt::fmt_clear_line   clear_line_before{ 1 };
	const fmt::fmt_clear_line   clear_line{2};
	

	const fmt::fmt_style_text reset_text_sytle{};


	const fmt::fmt_style_text set_text_bold{ 1 };
	const fmt::fmt_style_text set_text_dim{ 2 };
	const fmt::fmt_style_text set_text_italic{3};
	const fmt::fmt_style_text set_text_underline{4};
	const fmt::fmt_style_text set_text_blink{ 5 };
	const fmt::fmt_style_text set_text_strike{9};

	const fmt::fmt_style_text clear_text_bold{ 22 };
	const fmt::fmt_style_text clear_text_dim{ 22 };
	const fmt::fmt_style_text clear_text_italic{ 23 };
	const fmt::fmt_style_text clear_text_underline{ 24 };
	const fmt::fmt_style_text clear_text_blink{ 25 };
	const fmt::fmt_style_text clear_text_strike{ 29 };
}
template<> struct std::formatter<SKC::fmt::ES> {
	using type = SKC::fmt::ES;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		std::string fmt_string = "\x1b[";
		return std::format_to(ctx.out(), "{}", fmt_string);
	}

}; 
template<> struct std::formatter<SKC::fmt::fmt_go_home> {
	using type = SKC::fmt::fmt_go_home;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "{}H", SKC::fmt::es);
	}
};
template<> struct std::formatter<SKC::fmt::fmt_move> {
	using type = SKC::fmt::fmt_move;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		std::string fmt_string = std::format("{}", obj.lines);
		auto dir = obj.dir; 
		if (dir == SKC::fmt::move_dir_t::UP) {
			fmt_string += 'A'; 
		}
		if (dir == SKC::fmt::move_dir_t::DOWN) {
			fmt_string += 'B';
		}
		if (dir == SKC::fmt::move_dir_t::RIGHT) {
			fmt_string += 'C';
		}
		if (dir == SKC::fmt::move_dir_t::LEFT) {
			fmt_string += 'D';
		}
		return std::format_to(ctx.out(), "{}{}", SKC::fmt::es, fmt_string);
	}
};
template<> struct std::formatter<SKC::fmt::fmt_jump_to> {

	using type = SKC::fmt::fmt_jump_to;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "{}{}{}f", SKC::fmt::es, obj.line, obj.col);
	}
};
template<> struct std::formatter<SKC::fmt::fmt_save_loc> {

	using type = SKC::fmt::fmt_save_loc;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "{}s", SKC::fmt::es);
	}
};
template<> struct std::formatter<SKC::fmt::fmt_restore_loc> {

	using type = SKC::fmt::fmt_restore_loc;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "{}u", SKC::fmt::es);
	}
};
template<> struct std::formatter<SKC::fmt::fmt_clear_screen> {

	using type = SKC::fmt::fmt_clear_screen;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		if (obj.mode == 0) {
			return std::format_to(ctx.out(), "{}J", SKC::fmt::es);
		}
		else {
			return std::format_to(ctx.out(), "{}{}J", SKC::fmt::es, obj.mode);
		}
	}
};
template<> struct std::formatter<SKC::fmt::fmt_clear_line> {

	using type = SKC::fmt::fmt_clear_line;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		if (obj.mode == 0) {
			return std::format_to(ctx.out(), "{}K", SKC::fmt::es);
		}
		else {
			return std::format_to(ctx.out(), "{}{}K", SKC::fmt::es, obj.mode);
		}
	}
}; 
template<> struct std::formatter<SKC::fmt::fmt_style_text> {

	using type = SKC::fmt::fmt_style_text;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		return std::format_to(ctx.out(), "{}{}m", SKC::fmt::es, obj.mode);
	}
};
template<> struct std::formatter<SKC::fmt::fmt_set_palleted_color> {

	using type = SKC::fmt::fmt_set_palleted_color;
	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		return pos;
	}
	auto format(const type& obj, std::format_context& ctx) const {
		auto bg_color = obj.bg_color;
		if (bg_color > 9 || bg_color < 0) bg_color = 9;
		bg_color += 40; 
		auto fg_color = obj.fg_color;
		if (fg_color > 9 || fg_color < 0) bg_color = 9;
		fg_color += 30; 
		return std::format_to(ctx.out(), "{}{};{};{}m", SKC::fmt::es, obj.mode,   fg_color, bg_color);
	}
};
#pragma warning( pop ) // Restore the previous warning state
