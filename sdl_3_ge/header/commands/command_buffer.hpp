#pragma once

#include <deque>
#include <string>
#include "../thread/owning_mutex.hpp"
namespace skc::PRIVATE {
	class command_buffer {
		using T = std::string;
		SKC::threading::owning_mutex<std::deque<T>> m_cmd;
	public:
		void add_command(T command) {
			auto lg = std::lock_guard(m_cmd); 
			m_cmd.get().push_back(command);

		}
		[[nodiscard]] bool is_empty() {
			auto lg = std::lock_guard(m_cmd);
			return m_cmd.get().empty();

		}
		[[nodiscard]] T pop_front() {
			auto lg = std::lock_guard(m_cmd);
			auto& cmd_buff = m_cmd.get();
			auto t = cmd_buff.front();
			cmd_buff.pop_front();
			return t;
		}
	};
}
namespace skc {
	PRIVATE::command_buffer command_buffer;
}
