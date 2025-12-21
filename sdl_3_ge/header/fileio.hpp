#pragma once 
#include <filesystem> 
#include <string>
#include <bit>
#include <expected> 
#include <cstdio>



namespace SKC{
	class file_reader {
		std::filesystem::path m_file_path{};
		FILE* m_file_stream{0};
		constexpr static bool ENDIAN_IS_BIG = std::endian::native == std::endian::big; 
		
		struct error_flags_t {
			bool open{ true }, eof{ false }, good{ false };
			error_flags_t(const file_reader& fr) {
				open = fr.is_open();
				eof = fr.is_eof(); 
				good = fr.is_good();
			}
		};
		template	<typename T>
		using read_result = std::expected<T, error_flags_t>;

		template<typename T>
		read_result<T>  read_big_endian() {
			if constexpr (sizeof(T) == 1){
				int c = fgetc(m_file_stream);
				auto err = error_flags_t(*this);
				if (not err.good) {
					return std::unexpected{ err }; 
				}
				return (T)c; 
			}
			else {
				
				T value{};
				size_t read_count = fread(&value, sizeof(T), 1, m_file_stream);
				auto err = error_flags_t(*this);
				if (not err.good) {
					return std::unexpected{ err };
				}
				return value;
			}


		}
		auto read_char() {
			auto c = fgetc(m_file_stream);
			return c != EOF ? (char)c : 0;
		}
	public:


		file_reader(std::filesystem::path file_path) : m_file_path{ file_path } {
			fopen_s(&m_file_stream, m_file_path.string().c_str(), "r");
		}

		file_reader() = default;
		file_reader(const file_reader&) = delete;
		file_reader(file_reader&&) = delete;
		file_reader& operator=(const file_reader&) = delete;
		file_reader& operator=(file_reader&&) = delete;
		~file_reader()
		{
			if (m_file_stream == nullptr)return; 
			fclose(m_file_stream);
		}
		void open(std::filesystem::path file_path) {
			if (is_open()) {
				return; 
			}
			m_file_path = file_path;
			fopen_s(&m_file_stream, m_file_path.string().c_str(), "r");
		}
		
		[[nodiscard]] inline bool is_open() const noexcept {
			return m_file_stream != nullptr; 
		}
		[[nodiscard]] inline bool is_eof() const noexcept {
			if (not is_open()) return true; 
			return feof(m_file_stream) != 0; 
		}
		[[nodiscard]] inline bool is_good() const noexcept {
			return is_open() and not is_eof() and not ferror(m_file_stream); 
		}	
		[[nodiscard]] inline bool is_errored() const noexcept {
			if (not is_open() ) return true; 
			return ferror(m_file_stream) != 0; 
		}

		template <typename T, bool BIG = ENDIAN_IS_BIG>
		read_result<T> read() noexcept(std::is_trivial<T>::value) {
			if constexpr (BIG) {
				//this is the big endian case
				return read_big_endian<T>(); 
			}
			else {
				auto be_read = read_big_endian<T>();
				if (not be_read) return std::unexpected{ be_read.error() };
				return std::byteswap(be_read.value());	
			}
		}
				
		read_result<std::string>  read_string(size_t str_len) {
			std::string ret(str_len+1, 0);
			for (size_t i = 0; i < str_len; ++i) {
				auto char_res = read_char();
				ret[i] = char_res;
			}
			return ret;
		}
		void jump(long offset) {
			fseek(m_file_stream, offset, SEEK_CUR);
		}
	};
}