#pragma once
#include "../command_buffer.hpp"
#include "../command.hpp"

#include "../../GE/Window.hpp"
#include "../../util.hpp"

namespace skc::commands {


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
			size_t param_count = command.args();
			if (param_count == 0) {
				window.draw_texture(tid);
				return true;
			}

			switch (param_count) {
			case 2: { //x y 
				SKC::GE::Frect p{
					.x = skc::util::to_number<float>(command.consume()),
					.y = skc::util::to_number<float>(command.consume()),
					.w = 0.f, 
					.h = 0.f
				};
				window.draw_texture(tid, p);
				return true;
			}
			case 4: {// x y w h 
				SKC::GE::Frect p{
					.x = skc::util::to_number<float>(command.consume()),
					.y = skc::util::to_number<float>(command.consume()),
					.w = skc::util::to_number<float>(command.consume()),
					.h = skc::util::to_number<float>(command.consume())
				};
				window.draw_texture(tid, p);
				return true;
			}
			default: {
				return true;

			}
			}

		}
		return true; 
	}

}