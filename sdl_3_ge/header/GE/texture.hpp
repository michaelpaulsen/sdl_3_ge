#pragma once 
#include <SDL3/SDL.h>
#include <filesystem>

namespace SKC::GE {
	namespace fs = std::filesystem;

	struct SDL_image_texture_wrapper {
		inline static size_t next_tid = 0;
		SDL_Texture* tex;
		fs::path path;
		size_t tid;
		SDL_image_texture_wrapper(SDL_Texture* _t, fs::path pth = fs::path()) : tid(++next_tid), tex(_t), path(pth) {}
		~SDL_image_texture_wrapper() {
			SDL_DestroyTexture(tex);
		}
		SDL_image_texture_wrapper(SDL_image_texture_wrapper&& other) noexcept {
			tid = other.tid;
			tex = other.tex;
			other.tex = nullptr;
			other.tid = 0;

		}
		//NOTE(skc) we may have to implement these; 
		SDL_image_texture_wrapper(const SDL_image_texture_wrapper& other) = delete;
		SDL_image_texture_wrapper operator=(const SDL_image_texture_wrapper&) = delete;
		SDL_image_texture_wrapper operator=(SDL_image_texture_wrapper&& other) noexcept
		{
			if (this == &other) return std::move(*this); //self assignment check
			this->~SDL_image_texture_wrapper(); //destroy the current object
			tid = other.tid;
			this->path = other.path; //copy the path
			this->tex = other.tex; //move the texture pointer
			other.tex = nullptr; //nullify the other texture pointer
			tex = other.tex;
			return std::move(*this);
		};

		bool operator ==(const SDL_image_texture_wrapper& other) const { return path == other.path; }
		bool operator ==(const fs::path& other) const { return path == other; }
		bool operator== (size_t other) const { return tid == other; }

	};
}