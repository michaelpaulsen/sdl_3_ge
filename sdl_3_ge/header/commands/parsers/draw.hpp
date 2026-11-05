#pragma once
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
	}

	COMMAND_STATE tile_command(SKC::GE::window& window, skc::command& command) {
		auto tid = skc::util::to_number<size_t>(command.consume());
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
		return COMMAND_STATE::INVALID;
	}

	COMMAND_STATE draw_command(SKC::GE::window& window, skc::command& command) {
		auto sub_command = command.consume();
		if (sub_command == "rect") {
			size_t param_count = command.args();
			if (param_count == 4) {
				auto x = skc::util::to_number<double>(command.consume());
				auto y = skc::util::to_number<double>(command.consume());
				auto w = skc::util::to_number<double>(command.consume());
				auto h = skc::util::to_number<double>(command.consume());
				window.fill_rectangle(x, y, w, h);
				return COMMAND_STATE::PARSED;
			}
			return COMMAND_STATE::INVALID;
		}
		if (sub_command == "texture") {
			auto tid = skc::util::to_number<size_t>(command.consume());
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
		return COMMAND_STATE::INVALID;
	}

}