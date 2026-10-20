#pragma once

#include <deque>
#include <string>
#include "../thread/smart_lock.hpp"
namespace skc::PRIVATE {
	class command_buffer {
		using T = std::string;
		SKC::threading::smart_lock<std::deque<T>> m_cmd;
	public:
		void add_command(T command) {
			m_cmd.lock();
			m_cmd.get().push_back(command);
			m_cmd.unlock();

		}
		[[nodiscard]] bool is_empty() {
			m_cmd.lock();
			auto empty = m_cmd.get().empty();
			m_cmd.unlock();
			return empty;

		}
		[[nodiscard]] T pop_front() {
			m_cmd.lock();
			auto& cmd_buff = m_cmd.get();
			auto t = cmd_buff.front();
			cmd_buff.pop_front();
			m_cmd.unlock();
			return t;
		}
	};
}
namespace skc {
	PRIVATE::command_buffer command_buffer;
}
