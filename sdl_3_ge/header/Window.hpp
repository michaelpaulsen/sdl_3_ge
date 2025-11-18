#pragma once
#include <string>
#include <filesystem>
#include <map>
#include <vector>
#include <ranges>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "font_options.hpp"
#include "Color.hpp"
namespace SKC::GE {

	
	using point  = SDL_Point;
	using rect   = SDL_Rect; 
	using Fpoint = SDL_FPoint; 
	using Frect  = SDL_FRect;
	//TODO (skc) : move all of this filesystem 
	//stuff to own file. 
	namespace fs = std::filesystem; 
	using path_t = fs::path; 
	class window {
	protected:
		
		struct SDL_image_texture_wrapper {
			inline static size_t next_tid = 0;
			SDL_Texture* tex;
			fs::path path;
			size_t tid;
			SDL_image_texture_wrapper(SDL_Texture* _t, fs::path pth = fs::path()) : tid(++next_tid), tex(_t), path(pth) {

			}
			~SDL_image_texture_wrapper() {
				SDL_DestroyTexture(tex);
			}
			SDL_image_texture_wrapper(SDL_image_texture_wrapper&& other) noexcept {
				tid = ++next_tid;
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
				tid = ++next_tid;
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
		using c_t = Uint8;
		std::vector< SDL_image_texture_wrapper> m_image_textures{};
		int m_x{}, m_y{}, m_width, m_height;
		std::string title;
		float m_scale_x{ 1 }, m_scale_y{ 1 };
		SDL_Window* m_window;
		SDL_Renderer* m_renderer; 
		
		color m_background_color{}; 
		bool m_is_screen_saver_enabled = true; 
		
		//font stuff. 
		path_t m_font_dir; 
		TTF_TextEngine* m_text_engine; 

		SDL_Surface* blit_surface_with_resize(SDL_Surface* dst, SDL_Surface* src, int x, int y) {
			int needed_width = x + src->w;
			int needed_height = y + src->h;
			//if we don't need to resize the surface then we should just use it.
			rect src_rect = { x,y,src->w,src->h }; 
			bool x_is_positive = (x > 0);
			bool y_is_positive = (y > 0);

			bool both_cords_are_positive = x_is_positive && y_is_positive;
			bool needs_resize = (needed_height > dst->h || needed_width > dst->w);
			if ((!needs_resize) && both_cords_are_positive) {
				SDL_BlitSurface(src, NULL, dst, &src_rect);
				return dst; 
			}

			SDL_Rect dst_rect = { 0,0,dst->w,dst->h };
			if (needed_height < dst->h) needed_height = dst->h;
			if (needed_width < dst->w) needed_width  = dst->w;
			if (!x_is_positive) { needed_width += -x; }
			//if the y coordinate is negative then we need to add the height of the source surface to the destination surface height
			if (!y_is_positive)	 { 
				dst_rect.y = src->h + -y;
				needed_height += -y; 

			}

			auto new_surface = SDL_CreateSurface(needed_width, needed_height, dst->format);
			if (!new_surface) return dst; // if we cannot make a new surface than just return the old one 
			//TODO(skc) : make this fail some way...
			if (!x_is_positive) {
				dst_rect.x = (-x);
				src_rect.x = 0; 
			}
			

			SDL_BlitSurface(dst, NULL, new_surface, &dst_rect);
			SDL_BlitSurface(src, NULL, new_surface, &src_rect);
			SDL_DestroySurface(dst); 
			return new_surface; 

			
		}

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
#pragma region --The Non-rendering API--
		
		
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
		//TODO (skc) : create a resource class. 
		auto create_texture_from_path(std::filesystem::path pth) {
			SDL_Surface* surface = IMG_Load(pth.generic_string().c_str());
			if (!surface) return 0ull;
			auto tex = create_texture_from_surface(surface);
			SDL_DestroySurface(surface);
			m_image_textures.emplace_back(tex, pth);
			return m_image_textures.back().tid; 
				//return tex; 

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
		void set_background_color(c_t r, c_t g, c_t b, c_t a = 255) {
			m_background_color = { r,g,b,a }; 
		}

		void set_draw_color(color color) {
			SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, 255);
		}
		void set_draw_color(c_t r, c_t g, c_t b, c_t a = 255) {
			SDL_SetRenderDrawColor(m_renderer, r, g, b, a); 
		}
		
		void set_render_scale(float new_scale) {
			//if the user is not actually changing the scale there's no work to be done.
			if (new_scale == m_scale_x && new_scale == m_scale_y) return;
			
			m_scale_x = new_scale;
			m_scale_y = new_scale;
			SDL_SetRenderScale(m_renderer, new_scale, new_scale); 
		}
		void set_render_scale(float new_x_scale, float new_y_scale) {
			m_scale_x = new_x_scale;
			m_scale_y = new_y_scale;
			SDL_SetRenderScale(m_renderer, new_x_scale, new_y_scale);
		}
		
		const std::string get_window_title() const {
			return SDL_GetWindowTitle(m_window);
		}
		void set_title(std::string title) {
			SDL_SetWindowTitle(m_window, title.c_str());
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
#pragma endregion
		/*
		* --Intrinsics API-- 
		* These are the functions that change the contents of the screen. 
		*/
#pragma region --Intrinsics API--
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
			Uint8 r, g, b, a;
			SDL_GetRenderDrawColor(m_renderer, &r, &g, &b, &a);
			SDL_SetRenderDrawColor(m_renderer, m_background_color.r, m_background_color.g, m_background_color.b, m_background_color.a);
			SDL_RenderClear(m_renderer);
			SDL_SetRenderDrawColor(m_renderer, r, g, b, a);

		};
		void present() {
			SDL_RenderPresent(m_renderer);
		}
		
#pragma endregion
		/*
		*--texture API-- 
		*this is where all of the code 
		*that involves rendering textures goes 
		*/
		
		//NOTE(skc): YOU REALLY SHOULD NOT USE THIS MEMBER FUNCTION (AS IT RETURNS A RAW POINTER!!!).
		//it is only public for the SDL functions that are not wrapped in the ID based API yet. 
		//once I am sure that all of the SDL functions are wrapped in the ID based API I will make this private.
		//I make no guarantees that this will not cause a use after free error.
		//Though it should not be possible based off of how classes work in C++.
#pragma region --SDL_Texture* API--
		auto get_tex_from_tid(size_t tid) {
			for (const auto& texture : m_image_textures) {
				if (texture == tid) {
					return texture.tex;
				}
			}
			return (SDL_Texture *)nullptr; //return nullptr if not found
		}
		
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
#pragma endregion
//The ID based Texture format API 
		//NOTE(skc): this is the API that you should be using by default. the only reason 
		//you would use the SDL_Texture* API is if you are using a texture that is generated progrimatically.
#pragma region --ID based Texture API--
		auto get_texture_width(size_t tid) {
			auto txt = get_tex_from_tid(tid);
			if(!txt) return 0; //if the texture is not found then return 0
			return txt->w; 
		}
		auto get_texture_height(size_t tid) {
			auto txt = get_tex_from_tid(tid);
			if (!txt) return 0; //if the texture is not found then return 0
			return txt->h;
		}
		void draw_texture(size_t tid) {
			auto txt = get_tex_from_tid(tid); 
			if (!txt) return; 
			SDL_RenderTexture(m_renderer, txt, NULL, NULL);
		}
		void draw_texture(size_t tid, Frect pos) {
			auto txt = get_tex_from_tid(tid);
			if (!txt) return;
			SDL_RenderTexture(m_renderer, txt, NULL, &pos);
		}
		void draw_texture(size_t tid, Frect pos, Frect atlas_pos) {
			auto txt = get_tex_from_tid(tid);
			if (!txt) return;
			SDL_RenderTexture(m_renderer, txt, &atlas_pos, &pos);
		}
		void draw_texture_with_afine_transform(size_t tid, Fpoint tl, Fpoint tr, Fpoint bl) {
			auto txt = get_tex_from_tid(tid);
			if (!txt) return;
			SDL_RenderTextureAffine(m_renderer, txt, NULL, &tl, &tr, &bl);
		}
		void draw_texture_rotated(size_t tid, Frect atlas_pos, Frect pos, double angle, SDL_FlipMode flip = SDL_FLIP_NONE) {
			auto txt = get_tex_from_tid(tid);
			if (!txt) return;
			SDL_RenderTextureRotated(m_renderer, txt, &atlas_pos, &pos, angle, NULL, flip);
		}
		void draw_texture_rotated(size_t tid, const Frect pos, const double angle, const SDL_FlipMode flip = SDL_FLIP_NONE) {
			auto txt = get_tex_from_tid(tid);
			if (!txt) return;
			SDL_RenderTextureRotated(m_renderer, txt, NULL, &pos, angle, NULL, flip);
		}
		void draw_texture_rotated(size_t tid, const Frect atlas_pos, const Frect pos, const Fpoint center, const double angle, SDL_FlipMode flip = SDL_FLIP_NONE) {
			auto txt = get_tex_from_tid(tid);
			if (!txt) return;
			SDL_RenderTextureRotated(m_renderer, txt, &atlas_pos, &pos, angle, &center, flip);

		}
#pragma endregion
		/*
		*--TEXT API--
		* this is where all of the code for Text rendering goes
		*/
#pragma region --Text API--
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
		bool render_text_centered_simple(std::string text,
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
			draw_texture(text_texture, Frect(x - (texture_w / 2.0f), y - (texture_h / 2.0f), (float)texture_w, (float)texture_h));
			SDL_DestroyTexture(text_texture);
			return true;
		}
#pragma endregion
		bool render_text(std::string text, TTF_Font* font, font_options options) {
			std::vector<std::string> lines;

			if (TTF_GetFontSize(font) <= 0) return false; 

			if (text.empty())return false;

			//if we are told to not seperate any lines then we just render the text as a single line
			if(options.line_separator == font_options::LINE_SEPERATOR_NONE) {
				//if the line separator is none then we just render the text as a single line
				if (options.text_alignment == options.TEXT_ALIGNMENT_CENTER) {
					return render_text_centered_simple(
						text,
						font,
						options.x,
						options.y,
						options.color.r,
						options.color.g,
						options.color.b,
						options.color.a
					);
				}
				return render_text_simple(
					text,
					font,
					options.x,
					options.y,
					options.color.r,
					options.color.g,
					options.color.b,
					options.color.a
				);
			} 
			namespace rng = std::ranges;
			
			namespace views = std::ranges::views;
			if (options.line_separator == font_options::LINE_SEPARATOR_NEWLINE) {
				lines = rng::to<std::vector<std::string>>(rng::views::split(text, '\n')
					| views::transform([](auto&& str) { return std::string(str.begin(), str.end()); }));
			}
			else if (options.line_separator == font_options::LINE_SEPARATOR_CRLF) {
				lines = rng::to<std::vector<std::string>>(rng::views::split(text, "\r\n")
					| views::transform([](auto&& str) { return std::string(str.begin(), str.end()); }));
			}
			
			float line_y = 0; 
			SDL_Surface* text_surface = nullptr;
			for(const auto& line : lines) {
				auto text_line_surface = TTF_RenderText_Blended(font, line.c_str(), line.size(),
					{ options.color.r,options.color.g,options.color.b,options.color.a });
				if(!text_line_surface) {
					//TODO(skc) : handle error
					return false; 
				}
				if(!text_surface) {
					text_surface = SDL_CreateSurface(text_line_surface->w, text_line_surface->h, text_line_surface->format);
					SDL_BlitSurface(text_line_surface, NULL, text_surface, NULL);
					if (!text_surface) { return false; } 
				}
				else {
					auto x = 0;
					if(options.text_alignment == font_options::TEXT_ALIGNMENT_CENTER) {
						auto tw1 = text_surface->w, tw2 = text_line_surface->w;
						if(tw1 != tw2) {
							x = (tw1 - tw2)/2; //center the text
						}
					}
					text_surface = blit_surface_with_resize(text_surface, text_line_surface, x, (int)line_y);
				}
				
				if(options.line_height_mode == font_options::LINE_HEIGHT_MODE_ADDITIVE) {
					line_y += TTF_GetFontSize(font) + options.line_hight; //add the line height to the y position
				}
				else if (options.line_height_mode == font_options::LINE_HEIGHT_MODE_MULTIPLICATIVE) {
					line_y += TTF_GetFontSize(font) * options.line_hight; //add the line height to the y position

				}
				SDL_DestroySurface(text_line_surface);
			}
			if (!text_surface) return false;
			auto texture_w = text_surface->w;
			auto texture_h = text_surface->h;
			if (options.width != 0) texture_w = (int)options.width;
			if (options.height != 0) texture_h = (int)options.height;
			auto text_texture = SDL_CreateTextureFromSurface(m_renderer, text_surface);
			//doing this here so that the surface gets freed
			//no matter what
			SDL_DestroySurface(text_surface);
			if (!text_texture) return false;
			Frect pos = {options.x, options.y, (float)texture_w, (float)texture_h };
			/*
			   anchor point translation
			---------------------------
				AP_FLOATING (w*ox, h*oy)
				AP_TOP_LEFT ( 0  ,  0  )
			 AP_CENTER_LEFT	( 0  , -h/2)
			 AP_BOTTOM_LEFT	( 0  , -h  ) 
			   AP_TOP_RIGHT (-w  ,  0  )
			AP_CENTER_RIGHT (-w  , -h/2)
			AP_BOTTOM_RIGHT (-w  , -h  )
			      AP_CENTER (-w/2, -h/2)
			*/
			switch (options.anchor_point) {
			case font_options::AP_FLOATING:
				pos.x += options.positionoffset_x * texture_w;
				pos.y += options.positionoffset_y * texture_h;
				break;
				case font_options::AP_CENTER_LEFT:
					pos.y -= (float)(texture_h) / 2.0f;
					break;
				case font_options::AP_BOTTOM_LEFT:
					pos.y -= (float)(texture_h);
					break;
				case font_options::AP_TOP_RIGHT:
					pos.x -= (float)(texture_w);
					break;
				case font_options::AP_CENTER_RIGHT:
					pos.x -= (float)(texture_w);
					pos.y -= (float)(texture_h) / 2.0f;
					break;
				case font_options::AP_BOTTOM_RIGHT:
					pos.x -= (float)(texture_w);
					pos.y -= (float)(texture_h);
					break;
				case font_options::AP_CENTER:
					pos.x -= (float)(texture_w) / 2.0f;
					pos.y -= (float)(texture_h) / 2.0f;
					break;
			}
			auto rt = SDL_RenderTexture(m_renderer, text_texture, NULL, &pos);
			SDL_DestroyTexture(text_texture);
			return rt; 
		}
	};
}