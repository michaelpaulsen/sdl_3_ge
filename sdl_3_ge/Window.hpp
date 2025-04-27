#pragma once
#include <string>
#include <filesystem>
#include <SDL3/SDL.h>
namespace SKC::GE {
	//TODO(skc):move to own file. 
	struct color{
		using c_t = Uint8;
		c_t r{}, g{}, b{}, a{ 255 };
	};
	
	using  point = SDL_Point;
	using rect = SDL_Rect; 
	using Fpoint = SDL_FPoint; 
	using Frect = SDL_FRect;
	
	//TODO (skc) : move all of this filesystem 
	//stuff to own file. 
	namespace fs = std::filesystem; 
	using path_t = fs::path; 
	
	class window {
	protected: 
		using c_t = Uint8;

		 
		int m_x{}, m_y{}, m_width, m_height;
		std::string title; 
		
		SDL_Window* m_window;
		SDL_Renderer* m_renderer; 
		
		color m_background_color{}; 
		bool m_is_screen_saver_enabled = true; 
		
		//font stuff. 
		path_t m_font_dir; 
		TTF_TextEngine* m_text_engine; 
	public: 
		window(std::string title, int  width, int height, SDL_WindowFlags flags) : m_width { width }, m_height{ height } {
			SDL_CreateWindowAndRenderer(title.c_str(), width, height, flags, &m_window, &m_renderer);
			m_font_dir = path_t("./fonts"); 
			m_text_engine = TTF_CreateRendererTextEngine(m_renderer); 
		}
		~window() {
			SDL_DestroyRenderer(m_renderer);
			SDL_DestroyWindow(m_window);
		}
		
		
		/*
		* --The Non-rendering API--
		* this is where the getters and setters are
		* this is all code that isn't involved directly with the rendering
		*/
		auto get_window_surface() {
			return SDL_GetWindowSurface(m_window); 
		}
		auto get_renderer() {
			return m_renderer; 
		}

		
		void enable_screen_saver() {
			SDL_EnableScreenSaver();
			m_is_screen_saver_enabled = true; 
		}
		void disable_screen_saver() {
			SDL_DisableScreenSaver(); 
			m_is_screen_saver_enabled = false;

		}
		bool is_screen_saver_enabled() const {
			return m_is_screen_saver_enabled;
		}

		auto create_texture_from_surface(SDL_Surface *surface) {
			return SDL_CreateTextureFromSurface(m_renderer, surface);
		}

		void update_window_size() {
			rect ret{};
			SDL_GetRenderSafeArea(m_renderer, &ret);
			m_x = ret.x;
			m_y = ret.y;
			m_width = ret.w;
			m_height = ret.h;
		}
		
		rect get_window_rect() const{
			return { m_x, m_y, m_width,m_height }; 
		}
		
		void get_window_dimentions(int& width, int& height) const {
			width = m_width;
			height = m_height;
		}

		
		void set_background_color(color other) {
			m_background_color = other;
		}
		void set_background_color(c_t r, c_t g, c_t b) {
			m_background_color = { r,g,b,255 }; 
		}

		void set_draw_color(color color) {
			SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, 255);
		}
		void set_draw_color(c_t r, c_t g, c_t b, c_t a = 255) {
			SDL_SetRenderDrawColor(m_renderer, r, g, b, a); 
		}
		
		void set_window_render_scale(float new_scale) {
			SDL_SetRenderScale(m_renderer, new_scale, new_scale); 
		}
		void set_window_render_scale(float new_x_scale, float new_y_scale) {
			SDL_SetRenderScale(m_renderer, new_x_scale, new_y_scale);
		}
		double from_normilzed_width(double U) const {
			return U * (double)m_width; 
		}
		double to_normilzed_width(int px) const {
			return (double)px / (double)m_width;
		}
		double from_normilzed_height(double V) const {
			return V * (double)m_height;
		}
		double to_normilzed_height(int py) const{
			return (double)py / (double)m_height;
		}
		
		color get_draw_color() {
			color color = {}; 
			SDL_GetRenderDrawColor(m_renderer, &color.r, &color.g, &color.b, &color.a); 
			return color;  
		}
		
		/*
		* --Intrinsics API-- 
		* These are the functions that change the contents of the screen. 
		*/
		
		void draw_rectangle(Frect rect) {
			SDL_RenderRect(m_renderer, &rect);
		}
		void draw_rectangle(float x, float y, float w, float h) {
			Frect rect = { x,y,w,h }; 
			SDL_RenderRect(m_renderer, &rect);
		}
		
		void fill_rectangle(Frect rect) {
			SDL_RenderFillRect(m_renderer, &rect);
		}
		void fill_rectangle(float x, float y, float w, float h) {
			Frect rect = { x,y,w,h };
			SDL_RenderFillRect(m_renderer, &rect);
		}
		
		void draw_line(Frect rect) {
			SDL_RenderLine(m_renderer, rect.x, rect.y, rect.w, rect.h); 
		}
		void draw_line(Fpoint p1, Fpoint p2) {
			SDL_RenderLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
		}
		void draw_line(Fpoint p1, float x2, float y2) {
			SDL_RenderLine(m_renderer, p1.x, p1.y, x2, y2);
		}
		void draw_line(float x1, float y1 , float x2, float y2) {
			SDL_RenderLine(m_renderer, x1, y1, x2, y2);
		}
		void draw_line( float x2, float y2, Fpoint p1) {
			SDL_RenderLine(m_renderer, p1.x, p1.y, x2, y2);
		}

		void draw_pixel(Fpoint p) {
			SDL_RenderPoint(m_renderer, p.x, p.y);
		}
		void draw_pixel(float x, float y) {
			SDL_RenderPoint(m_renderer, x, y);
		}
		void clear() {
			SDL_SetRenderDrawColor(m_renderer, m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
			SDL_RenderClear(m_renderer);
		};
		void present() {
			SDL_RenderPresent(m_renderer);
		}

		
		/*
		*--texture API-- 
		*this is where all of the code 
		*that involves rendering textures goes 
		*/
		void draw_texture(SDL_Texture *txt) {
			SDL_RenderTexture(m_renderer, txt, NULL, NULL);
		}
		void draw_texture(SDL_Texture *txt, Frect pos) {
			SDL_RenderTexture(m_renderer, txt, NULL, &pos);
		}
		void draw_texture(SDL_Texture *txt, Frect pos, Frect atlas_pos) {
			SDL_RenderTexture(m_renderer, txt, &atlas_pos , &pos );
		}

		void draw_texture_with_afine_transform(SDL_Texture *txt, Fpoint tl, Fpoint tr, Fpoint bl ) {
			SDL_RenderTextureAffine(m_renderer, txt, NULL, &tl, &tr, &bl);
		}

		void draw_texture_rotated(SDL_Texture* txt, Frect atlas_pos, Frect pos, double angle, SDL_FlipMode flip = SDL_FLIP_NONE) {
			SDL_RenderTextureRotated(m_renderer, txt, &atlas_pos, &pos, angle, NULL, flip);
		}
		void draw_texture_rotated(SDL_Texture* txt, const Frect pos, const double angle, const SDL_FlipMode flip = SDL_FLIP_NONE) {
			SDL_RenderTextureRotated(m_renderer, txt, NULL, &pos, angle, NULL, flip); 
		}
		void draw_texture_rotated(SDL_Texture* txt, const Frect atlas_pos, const Frect pos, const Fpoint center, const double angle, SDL_FlipMode flip = SDL_FLIP_NONE) {
			SDL_RenderTextureRotated(m_renderer, txt, &atlas_pos, &pos, angle, &center, flip);

		}

		/*
		*--TEXT API--
		* this is where all of the code for Text rendering goes
		*/
		bool render_text_simple(std::string text,
			TTF_Font* font,
			float x, float y,
			c_t r = 255, c_t g = 255, c_t b = 255, c_t a = 255) {
			auto text_surface = TTF_RenderText_Blended(font,
				text.c_str(), text.size(),
				{ r,g,b,a });
			if (!text_surface) return false;
			auto texture_w = text_surface->w;
			auto texture_h = text_surface->h;
			auto text_texture = SDL_CreateTextureFromSurface(
				m_renderer, text_surface);
			//doing this here so that the surface gets freed 
			//no matter what
			SDL_DestroySurface(text_surface);
			if (!text_texture) return false;
			draw_texture(text_texture, Frect(x, y, (float)texture_w, (float)texture_h));
			SDL_DestroyTexture(text_texture);
			return true;
		}
	};
}