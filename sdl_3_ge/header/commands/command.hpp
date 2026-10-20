#pragma once
#include <string>
#include <vector>
namespace skc {
	class command {
		std::string                 m_str; 
		std::string                 m_command;
		std::vector<std::string>    m_args;
		size_t m_arg_ptr = 0;
	public:
		command(std::string line) {
			m_str = line;
			using namespace std::string_view_literals;
			size_t offset = 0; 
			while (isspace(line[offset++])) {};
			--offset;
			auto cmd_end = line.find(' ', offset);
			if (cmd_end == std::string::npos) {
				m_command = line;
				return; 
			}
			m_command = line.substr(offset, cmd_end); 

			std::string p;
			size_t x = offset + cmd_end; 
			while(x < line.size()){

				auto nx = x + 1; 
				if (line[nx] == 34){ 
					auto pos = line.find(34, nx + 1); 
					if (pos == std::string::npos) throw "invalid command missing closing \""; 
					//TODO(skc):implement escaping? 
					if (pos != line.size()-1 && !isspace(line[pos + 1])) exit(-1);// throw "invalid command closing quote not at end of paramiter"; 
					auto count = pos - nx;
					m_args.push_back(line.substr(nx + 1, count-1));
					x = pos;
					continue; 
				}
				auto pos = line.find(' ', nx);
				size_t count = 0; 
				if (pos == std::string::npos) { 
					count = line.size() - nx; 
				}
				else {
					count = pos - nx; 

				}
				auto word = line.substr(nx, count); 
				if (!word.empty()) {
					m_args.push_back(word);

				}
				x = pos; 
			}
		}
		auto cmd() const { return m_command; }
		auto str() const { return m_str; }
		auto total_args() const { return m_args.size(); }
		auto args() const { return m_args.size() - m_arg_ptr; }
		std::string peek() {
			return m_args.at(m_arg_ptr);
		}
		void skip(size_t count = 1) {
			m_arg_ptr += count;
		}
		auto consume() {
			return m_args.at(m_arg_ptr++);
		}
		auto begin() {
			return m_args.begin(); 
		}
		auto end() {
			return m_args.end();
		}
	};
}