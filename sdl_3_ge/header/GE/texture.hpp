#pragma once 
#include <SDL3/SDL_render.h>
#include <filesystem>
#include <utility>
#include <exception>

namespace SKC::GE {
	namespace fs = std::filesystem;
	
	class texture_wrapper {
		inline static size_t next_tid = 0;
		SDL_Texture* m_tex;
		fs::path     m_path;
		size_t       m_tid;
	public: 
		texture_wrapper(SDL_Texture* _t, fs::path pth = fs::path()) noexcept : 
			m_tid(++next_tid), m_tex(_t), m_path(pth)
		{
			//take ownership of the texture 
			_t = nullptr; 
		}
		~texture_wrapper() {
			SDL_DestroyTexture(m_tex);
			this->m_tex = nullptr; 
		}
		texture_wrapper(texture_wrapper&& other) noexcept {
			m_tid = other.m_tid;
			m_tex = other.m_tex;
			other.m_tex = nullptr;
			other.m_tid = 0;

		}
		[[nodiscard]] auto tid() const { return m_tid;}
		SDL_Texture* get() const noexcept{
			return m_tex; 
		}
		//NOTE(skc) we may have to implement these; 
		texture_wrapper          (const texture_wrapper&       ) = delete;
		texture_wrapper operator=(const texture_wrapper&       ) = delete;
		texture_wrapper operator=(      texture_wrapper&& other) noexcept{
			if (this == &other) return std::move(*this); //self assignment check
			this->~texture_wrapper(); //destroy the current object
			m_tid = other.m_tid;
			m_path = other.m_path; //copy the path
			m_tex  = other.m_tex; //move the texture pointer
			other.m_tex = nullptr; //nullify the other texture pointer
			return std::move(*this);
		};

		bool operator ==(const texture_wrapper& other) const { return m_path == other.m_path; }
		bool operator ==(const fs::path& other) const { 
			if (m_path.empty()) return false;
			return m_path == other;
		}
		bool operator== (size_t other) const { return m_tid == other; }
		operator bool() {
			return m_tex; 
		}
	};
}