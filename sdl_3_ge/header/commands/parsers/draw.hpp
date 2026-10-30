#pragma once
#include "../command_buffer.hpp"
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

	bool tile_command(SKC::GE::window& window, skc::command& command) {
		auto tid = skc::util::to_number<size_t>(command.consume());
		if (command.args() == 0) {
			window.draw_texture_tiled(tid);
			return true; 
		}
		auto scale = skc::util::to_number<float>(command.consume());
		if (command.args() == 0) {
			window.draw_texture_tiled(tid, scale);
			return true;
		}
		auto pos = PRIVATE::parse_frect(command); 
		std::print("pos : ({},{}) {}x{}", pos.x, pos.y, pos.w, pos.h);
		if (command.args() >= 2) {
			auto atlas = PRIVATE::parse_frect(command);
			window.draw_texture_tiled(tid, scale, pos, atlas); 
			return true; 
		}
		window.draw_texture_tiled(tid, scale, pos); 
		return true; 
	}

	bool draw_command(SKC::GE::window& window, skc::command& command) {
		auto sub_command = command.consume();
		if (sub_command == "rect") {
			size_t param_count = command.args();
			if (param_count == 4) {
				auto x = skc::util::to_number<double>(command.consume());
				auto y = skc::util::to_number<double>(command.consume());
				auto w = skc::util::to_number<double>(command.consume());
				auto h = skc::util::to_number<double>(command.consume());
				window.fill_rectangle(x, y, w, h);
				return true;
			}
			return true;
		}
		if (sub_command == "texture") {
			auto tid = skc::util::to_number<size_t>(command.consume());
			if (command.args() == 0) {
				window.draw_texture(tid);
				return true;
			}
			auto destination_rect = PRIVATE::parse_frect(command);
			if (command.args() >= 2) {
				auto source_rect = PRIVATE::parse_frect(command);
				window.draw_texture(tid, destination_rect, source_rect); 
				return true; 
			}
			window.draw_texture(tid, destination_rect);
			return true; 
		}
		return true; 
	}

}