#pragma once
#include <SDL3_ttf/SDL_ttf.h>
#include <filesystem>
namespace SKC::GE {
	enum class FONT_CHANGE_ERROR {
		E_OKAY,
		E_NO_CHANGE,
		E_SDL_ERROR
	};
	class font {
		std::filesystem::path m_path; 
		float m_size{ 0 };
		int m_outline{ 0 };
		TTF_Font *m_font; 
	public:
		font() = delete;
		font(std::filesystem::path path, float size): m_path(path),  m_size(size) {
			m_font = TTF_OpenFont(path.string().c_str(), size); 
		};
		font(font&) = delete; 
		font(font&& other) noexcept {
			m_path = other.m_path;
			m_font = other.m_font; 
			m_size = other.m_size; 
			m_outline = other.m_outline; 
			other.m_font = nullptr; 

		}
		~font()
		{
			TTF_CloseFont(m_font);
		}
		font operator=(font&) = delete; 
		void operator=(font&& other) noexcept {
			*this = std::move(other); 
		}
		
		void operator=(std::filesystem::path path) {
			
			TTF_CloseFont(m_font);
			m_font = TTF_OpenFont(path.string().c_str(), m_size);
			if (m_outline) {
				TTF_SetFontOutline(m_font, m_outline);
			}

		}
		
		void make_out_line_font(int outline_size) {
			TTF_SetFontOutline(m_font, outline_size); 
			m_outline = outline_size;
		}
		void set_size(float size) {
			if (size == m_size) return;
			m_size = size;
			TTF_SetFontSize(m_font, size); 
		}
		TTF_Font* operator*() {
			return m_font; 
		}
		bool operator!() {
			return !m_font;
		}
	};
}