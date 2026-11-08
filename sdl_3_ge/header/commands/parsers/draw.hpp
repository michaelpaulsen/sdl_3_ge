#pragma once
#include <unordered_map>


#include "../command_buffer.hpp"
#include "../command_state.hpp"
#include "../command.hpp"

#include "../../GE/Window.hpp"
#include "../../util.hpp"

namespace skc::commands {
	namespace PRIVATE {
		SKC::GE::Frect parse_frect(skc::command& command) {
			SKC::GE::Frect rect = { 0.f,0.f,0.f,0.f }; 
			if (command.args() >= 2) {
				rect.x = skc::util::to_number<float>(command.consume());
				rect.y = skc::util::to_number<float>(command.consume());
			}
			if (command.args() >= 2) {
				rect.w = skc::util::to_number<float>(command.consume());
				rect.h = skc::util::to_number<float>(command.consume());
			}
			return rect; 
		}
		size_t get_tid(
			command& command,
			const std::unordered_map<std::string, size_t>& tnames
		) {
			size_t tid = 1;
			auto tid_param = command.consume();
			if (util::is_num(tid_param)) {
				tid = util::to_number<size_t>(tid_param);
			}
			else {
				try {
					tid = tnames.at(tid_param);
				}
				catch (...) {
					return 1;
				}
			}
			return tid;
		}
	}


	COMMAND_STATE render_texture(
		SKC::GE::window& window,
		skc::command& command,
		const std::unordered_map<std::string, size_t>& tnames
	) {

		if (command.args() == 0) return COMMAND_STATE::INVALID;

		auto tid = PRIVATE::get_tid(command, tnames); 
		if (command.args() == 0) {
			window.draw_texture(tid);
			return COMMAND_STATE::PARSED;
		}
		auto destination_rect = PRIVATE::parse_frect(command);
		if (command.args() >= 2) {
			auto source_rect = PRIVATE::parse_frect(command);
			window.draw_texture(tid, destination_rect, source_rect);
			return COMMAND_STATE::PARSED;
		}
		window.draw_texture(tid, destination_rect);
		return COMMAND_STATE::PARSED;
	}



	COMMAND_STATE tile_command(
		SKC::GE::window& window,
		skc::command& command,
		const std::unordered_map<std::string, size_t> &tnames
	) {
		if (command.args() == 0) return COMMAND_STATE::INVALID; 
		size_t tid = PRIVATE::get_tid(command, tnames); 

		if (command.args() == 0) {
			window.draw_texture_tiled(tid);
			return COMMAND_STATE::PARSED; 
		}
		auto scale = skc::util::to_number<float>(command.consume());
		if (command.args() == 0) {
			window.draw_texture_tiled(tid, scale);
			return COMMAND_STATE::PARSED;
		}
		auto pos = PRIVATE::parse_frect(command); 
		if (command.args() >= 2) {
			auto atlas = PRIVATE::parse_frect(command);
			window.draw_texture_tiled(tid, scale, pos, atlas); 
			return COMMAND_STATE::PARSED;
		}
		window.draw_texture_tiled(tid, scale, pos); 
		return COMMAND_STATE::PARSED;
	}
}