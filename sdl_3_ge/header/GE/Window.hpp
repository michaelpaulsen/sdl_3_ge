#pragma once
#include <string>
#include <filesystem>
#include <vector>
#include <ranges>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL_rect.h>
#include <print>
#include <cstdio>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>

#include "../math/Vector.hpp"
#include "Color.hpp"
#include "font_options.hpp"
#include "texture.hpp"

#define PRIV_GET_TEX_(tid, name, ret) auto name = get_tex_from_tid(tid);\
                      if (!name) return ret;

#define PRIV_PUSH_RENDER_BLEND_MODE_()  SDL_BlendMode prev_mode{};\
					  SDL_GetRenderDrawBlendMode(m_renderer, &prev_mode)
#define PRIV_POP_RENDER_BLEND_MODE_() SDL_SetRenderDrawBlendMode(m_renderer, prev_mode)
#define PRIV_SET_RENDER_BLEND_MODE_()\
SDL_BlendMode tp ={};\
SDL_GetTextureBlendMode(SDL_GetRenderTarget(m_renderer), &tp);\
SDL_SetRenderDrawBlendMode(m_renderer, tp);

namespace SKC::GE {

	
	using point  = SDL_Point;
	using rect   = SDL_Rect; 
	using Fpoint = SDL_FPoint; 
	using Frect  = SDL_FRect;
	//TODO (skc) : move all of this filesystem 
	//stuff to own file. 
	namespace fs = std::filesystem; 
	class window {
	protected:
		
		using c_t = Uint8;
		using tick_t = Uint64; 
		std::vector< SDL_image_texture_wrapper> m_image_textures{};
		int m_x{}, m_y{}, m_width, m_height;
		std::string m_title;
		float m_scale_x{ 1 }, m_scale_y{ 1 };
		SDL_Window* m_window;
		SDL_Renderer* m_renderer; 
		
		color m_background_color{}; 
		bool m_is_screen_saver_enabled{ true },
			 m_limit_frame_rate{ true }, 
			m_is_fullscreen{ false },
			m_is_bordered{ true },
			m_is_visible{ true };
		
		tick_t last_frame_start{}, m_last_frame_time{};

		float m_frame_rate{ 30 };
		//NOTE(skc): I know that this is a code smell but this file is getting too big
		//also this function can be used outside of this class so it makes sense to 
		//have it in its own file.
		#include "./private/blit_surface_with_resize.hpp"

		Math::Vect2i get_size_of_uniform_atlas(SDL_Texture* txt, int size) {
			if (!txt) return { 0,0 };
			return Math::Vect2i{
				txt->w / size,
				txt->h / size
			};
		}
		auto get_tex_from_tid(size_t tid) {
			for (const auto& texture : m_image_textures) {
				if (texture == tid) {
					return texture.tex;
				}
			}
			return (SDL_Texture*)nullptr; //return nullptr if not found
		}
	public: 
		window(std::string title, int  width, int height, SDL_WindowFlags flags) noexcept  : m_width { width }, m_height{ height } {
			m_is_fullscreen = (flags & SDL_WINDOW_FULLSCREEN) != 0;
			m_is_bordered = (flags & SDL_WINDOW_BORDERLESS) == 0;
			m_is_visible = (flags & SDL_WINDOW_HIDDEN) == 0;
			m_image_textures.reserve(128);
			
			SDL_CreateWindowAndRenderer(title.c_str(), width, height, flags, &m_window, &m_renderer);
			if (not m_window || not m_renderer) {
				std::println(stderr, "[FATAL ERROR] could not create SDL_Window or SDL_Renderer: {}", SDL_GetError());
				std::exit(EXIT_FAILURE);
			}
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

		bool save_BMPscreenshot(fs::path pth) {
			auto pix_format = SDL_GetWindowSurface(m_window);
			auto render_data = SDL_RenderReadPixels(m_renderer,NULL); 
			auto surface = SDL_CreateSurfaceFrom(
				pix_format->w, pix_format->h,
				render_data->format, render_data->pixels, 
				render_data->pitch);
			auto ret = SDL_SaveBMP(surface, pth.generic_string().c_str());
			SDL_DestroySurface(surface);
			SDL_DestroySurface(render_data);
			return ret;
		}
		//TODO(skc): add getter and setters for window 
		//fullscreen display mode. 
		bool set_window_full_screen(bool is_fullscreen) {
			if (is_fullscreen == m_is_fullscreen) return false;
			m_is_fullscreen = is_fullscreen;
			return SDL_SetWindowFullscreen(m_window, is_fullscreen);
 		}
		bool set_window_border(bool is_bordered) {
			if (is_bordered == m_is_bordered) return false;
			m_is_bordered = is_bordered;
			return SDL_SetWindowBordered(m_window, is_bordered);
		}
	
		void enable_screen_saver() {
			SDL_EnableScreenSaver();
			m_is_screen_saver_enabled = true; 
		}
		void disable_screen_saver() {
			SDL_DisableScreenSaver(); 
			m_is_screen_saver_enabled = false;

		}
		[[nodiscard]] auto create_modifible_texture(int width, int height) {
			auto t = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_TARGET,
				width, height);
			if (not t ) return 0llu;
			SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
			m_image_textures.emplace_back(t);
			return m_image_textures.back().tid;
		}

		[[nodiscard]] auto create_modifible_texture() {
			update_window_size();
			return create_modifible_texture(m_width, m_height);
		}

		[[nodiscard]] auto create_texture_from_path(fs::path pth) {
			if (!fs::exists(pth) || fs::is_directory(pth)) {
				std::println(stderr, "[ERROR] trying to create texture with invalid path {}", pth.string());
				return 0ull; 
			}
			SDL_Surface* surface = IMG_Load(pth.generic_string().c_str());
			if (!surface) return 0ull;
			auto tex = SDL_CreateTextureFromSurface(m_renderer, surface);
			
			//NOTE(skc) doing this here so that the surface gets freed 
			//even if the texture creation fails.

			SDL_DestroySurface(surface);
			
			if (!tex) return 0ull;
			
			//NOTE(skc): 
			//default to blending mode for images with transparency
			//because this is the expected default behavior.
			//and if you don't want blending you can always change it later.
			SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
			m_image_textures.emplace_back(tex, pth);
			return m_image_textures.back().tid; 
		}
		void update_window_size() {
			rect ret{};
			SDL_GetRenderSafeArea(m_renderer, &ret);
			m_x = ret.x;
			m_y = ret.y;
			m_width = ret.w;
			m_height = ret.h;
		}
		
		rect get_window_rect() const{return { m_x, m_y, m_width,m_height };}
		
		void get_window_dimentions(int& width, int& height) const { SDL_GetWindowSize(m_window, &width, &height); }
		SKC::Math::Vect2i get_window_dimentions() const {
			int w, h;
			SDL_GetWindowSize(m_window, &w, &h);
			return { w,h }; 
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
		auto get_frame_rate()  const { return m_frame_rate; }
		bool is_screen_saver_enabled() const { return m_is_screen_saver_enabled; }
		bool is_visible() const { return m_is_visible; }
		bool is_window_bordered() const { return m_is_bordered; }
		bool is_window_fullscreen() const { return m_is_fullscreen; }
		const std::string get_window_title() const { return SDL_GetWindowTitle(m_window); }
		double from_normilzed_height(double V) const { return V * (double)m_height;}
		double from_normilzed_width(double U) const {return U * (double)m_width;}
		double to_normilzed_height(int py) const { return (double)py / (double)m_height; }
		double to_normilzed_width(int px) const { return (double)px / (double)m_width;}
		void disable_frame_rate_limiter() { m_limit_frame_rate = false; }
		void enable_frame_rate_limiter() { m_limit_frame_rate = true; }
		void set_background_color(c_t r, c_t g, c_t b, c_t a = 255) { m_background_color = { r,g,b,a }; }
		void set_background_color(color other) { m_background_color = other; }
		void set_draw_color(c_t r, c_t g, c_t b, c_t a = 255) { SDL_SetRenderDrawColor(m_renderer, r, g, b, a); }
		void set_draw_color(color color) { SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, 255); }
		void set_frame_rate(float nfps) { m_frame_rate = nfps; }
		void set_title(std::string title) { SDL_SetWindowTitle(m_window, title.c_str()); }

		color get_draw_color()  const {
			color color = {}; 
			SDL_GetRenderDrawColor(m_renderer, &color.r, &color.g, &color.b, &color.a); 
			return color;  
		}
		void start_of_frame() { last_frame_start = SDL_GetTicks(); }
		void end_frame() {
			auto now = SDL_GetTicks();
			m_last_frame_time = (now - last_frame_start);

		}

		void wait_for_frame() {
			if (!m_limit_frame_rate) return; 
			end_frame(); 
			auto frame_time = static_cast<Uint64>(1000.f / m_frame_rate);
			if (m_last_frame_time < frame_time) {
				SDL_Delay((Uint32)(frame_time - m_last_frame_time));
			}
			end_frame();
		}
		tick_t delta_time() const  { return m_last_frame_time; }
		void show() {
			if (m_is_visible) return; 
			SDL_ShowWindow(m_window);
			m_is_visible = true; 
		}
		void hide() {
			if (!m_is_visible) return; 
			SDL_HideWindow(m_window);
			m_is_visible = false; 
		}
#pragma endregion
		/*
		* --Intrinsics API-- 
		* These are the functions that change the contents of the screen. 
		*/
#pragma region --Intrinsics API--
		void draw_rectangle(Frect rect) {
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_(); 
				SDL_RenderRect(m_renderer, &rect);
			PRIV_POP_RENDER_BLEND_MODE_();

		}
		void draw_rectangle(float x, float y, float w, float h) const {
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			Frect rect = { x,y,w,h };
			SDL_RenderRect(m_renderer, &rect);
			PRIV_POP_RENDER_BLEND_MODE_();
		}
		
		void fill_rectangle(Frect rect)  const{
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			SDL_RenderFillRect(m_renderer, &rect);
			PRIV_POP_RENDER_BLEND_MODE_();

		}
		void fill_rectangle(float x, float y, float w, float h) const {
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			Frect rect = { x,y,w,h };
			SDL_RenderFillRect(m_renderer, &rect);
			PRIV_POP_RENDER_BLEND_MODE_();

		}
		using fpt = float; 

		void draw_line(Frect rect) const { 
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			SDL_RenderLine(m_renderer, rect.x, rect.y, rect.w, rect.h);
			PRIV_POP_RENDER_BLEND_MODE_();
		}

		void draw_line(Fpoint p1, Fpoint p2) const {
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			SDL_RenderLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
			PRIV_POP_RENDER_BLEND_MODE_();
		}

		void draw_line(Fpoint p1, float x2, float y2) const {
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			SDL_RenderLine(m_renderer, p1.x, p1.y, x2, y2);
			PRIV_POP_RENDER_BLEND_MODE_();
		}

		void draw_line(float x1, float y1 , float x2, float y2 ) const {
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			SDL_RenderLine(m_renderer, x1, y1, x2, y2);
			PRIV_POP_RENDER_BLEND_MODE_();
		}

		void draw_line(float x2, float y2, Fpoint p1) const {
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			SDL_RenderLine(m_renderer, p1.x, p1.y, x2, y2);
			PRIV_POP_RENDER_BLEND_MODE_();
		}
		
		
		void draw_pixel(Fpoint p)     { 
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			SDL_RenderPoint(m_renderer, p.x, p.y);
			PRIV_POP_RENDER_BLEND_MODE_();
		}
		void draw_pixel(float x, float y) {
			PRIV_PUSH_RENDER_BLEND_MODE_();
			PRIV_SET_RENDER_BLEND_MODE_();
			SDL_RenderPoint(m_renderer, x, y);
			PRIV_POP_RENDER_BLEND_MODE_();
		}
		void present() { SDL_RenderPresent(m_renderer); }
		void clear() {
			Uint8 r, g, b, a;
			SDL_GetRenderDrawColor(m_renderer, &r, &g, &b, &a);
			SDL_SetRenderDrawColor(
				m_renderer,
				m_background_color.r,
				m_background_color.g,
				m_background_color.b,
				m_background_color.a
			);
			SDL_RenderClear(m_renderer);
			SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
		};
		
#pragma endregion
		/*
		*--texture API-- 
		*this is where all of the code 
		*that involves rendering textures goes 
		*/
		
		
#pragma region --ID based Texture API--
		float get_texture_width(size_t tid) {
			PRIV_GET_TEX_(tid, txt, 0)
			float w{0};
			SDL_GetTextureSize(txt, &w, NULL); 
			return w; 
		}
		float get_texture_height(size_t tid) {
			PRIV_GET_TEX_(tid, txt, 0)
			float h{ 0 };
			SDL_GetTextureSize(txt, NULL, &h);
			return h;
		}
		
		skc_Vect2f get_texture_size(size_t tid) {
			PRIV_GET_TEX_(tid, txt, skc_Vect2f(0, 0));
			skc_Vect2f ret{0,0};
			SDL_GetTextureSize(txt, &ret.x, &ret.y);
			return ret; 
		}
		void set_texture_alpha_mod(size_t tid, uint8_t mod) {
			PRIV_GET_TEX_(tid,texture,)
			SDL_SetTextureAlphaMod(texture, mod);
		}
		void set_texture_blend_mode(size_t tid, SDL_BlendMode blend_mode) {
			PRIV_GET_TEX_(tid, texture,)
			SDL_SetTextureBlendMode(texture, blend_mode);
		}
		auto set_texture_color_mod(size_t tid, c_t r, c_t g, c_t b) {
			PRIV_GET_TEX_(tid, texture,false)
			return SDL_SetTextureColorMod(texture, r, g, b);
		}
		bool set_texture_scale_mode(size_t tid, SDL_ScaleMode scale_mode) {
			PRIV_GET_TEX_(tid, texture, false)
			return SDL_SetTextureScaleMode(texture, scale_mode);
		}
		void draw_texture(size_t tid) {
			auto txt = get_tex_from_tid(tid); 
			if (!txt) return; 
			SDL_RenderTexture(m_renderer, txt, NULL, NULL);
		}
		void draw_texture(size_t tid, Frect pos) {
			PRIV_GET_TEX_(tid, txt,)
			SDL_RenderTexture(m_renderer, txt, NULL, &pos);
		}
		void draw_texture(size_t tid, Frect pos, Frect atlas_pos) {
			PRIV_GET_TEX_(tid, txt,)
			SDL_RenderTexture(m_renderer, txt, &atlas_pos, &pos);
		}
		void draw_texture_with_afine_transform(size_t tid, Fpoint tl, Fpoint tr, Fpoint bl) {
			PRIV_GET_TEX_(tid, txt,)
			SDL_RenderTextureAffine(m_renderer, txt, NULL, &tl, &tr, &bl);
		}
		void draw_texture_rotated(size_t tid, Frect atlas_pos, Frect pos, double angle, SDL_FlipMode flip = SDL_FLIP_NONE) {
			PRIV_GET_TEX_(tid, txt,)
			SDL_RenderTextureRotated(m_renderer, txt, &atlas_pos, &pos, angle, NULL, flip);
		}
		void draw_texture_rotated(size_t tid, const Frect pos, const double angle, const SDL_FlipMode flip = SDL_FLIP_NONE) {
			PRIV_GET_TEX_(tid, txt,)
			SDL_RenderTextureRotated(m_renderer, txt, NULL, &pos, angle, NULL, flip);
		}
		void draw_texture_rotated(size_t tid, const Frect atlas_pos, const Frect pos, const Fpoint center, const double angle, SDL_FlipMode flip = SDL_FLIP_NONE) {
			PRIV_GET_TEX_(tid, txt,)
			SDL_RenderTextureRotated(m_renderer, txt, &atlas_pos, &pos, angle, &center, flip);

		}
		bool set_render_target(size_t tid) {
				//FIXME(skc): this should destinguish between the texture not
				//existing and the texture not being a render target

			if (tid == 0)  return SDL_SetRenderTarget(m_renderer, 0);
			PRIV_GET_TEX_(tid, txt, false)
			if ((SDL_GetTextureProperties(txt) | SDL_TEXTUREACCESS_TARGET) == 0) return false;
			return SDL_SetRenderTarget(m_renderer, txt);
		}
		Math::Vect2i get_size_of_uniform_atlas(size_t tid, int size) {
			PRIV_GET_TEX_(tid, txt, (Math::Vect2i{ 0,0 }))
			return get_size_of_uniform_atlas(txt, size);
		}
		//TODO(skc) : should make a class that stores Texture atlases 
		void draw_from_uniform_atlas(size_t tid, Frect pos, int size, size_t atlas_x, size_t atlas_y) {
			PRIV_GET_TEX_(tid, txt,)
			auto atlas_size = get_size_of_uniform_atlas(txt, size); 
			size_t ax = atlas_x % atlas_size.x;
			size_t ay = atlas_y % atlas_size.y;
			Frect atlas_pos = {
				(float)(ax * size),
				(float)(ay * size),
				(float)size,
				(float)size
			};
			SDL_RenderTexture(m_renderer, txt, &atlas_pos, &pos); 
		}
#pragma endregion
		/*
		*--TEXT API--
		* this is where all of the code for Text rendering goes
		*/
#pragma region --Text API--
		bool render_text_simple(std::string text,
			TTF_Font* font, float x, float y, color font_color
		) {
			auto text_surface = TTF_RenderText_Blended(
				font, text.c_str(), text.size(), font_color
			);
			if (!text_surface) return false;
			auto texture_w = text_surface->w;
			auto texture_h = text_surface->h;
			auto text_texture = SDL_CreateTextureFromSurface( m_renderer, text_surface);
			//doing this here so that the surface gets freed 
			//no matter what
			SDL_DestroySurface(text_surface);
			if (!text_texture) return false;
			auto rect = Frect(x, y, (float)texture_w, (float)texture_h);
			SDL_RenderTexture(m_renderer, text_texture, &rect, NULL);
			SDL_DestroyTexture(text_texture);
			return true;
		}
		bool render_text_centered_simple(
			std::string text, TTF_Font* font, float x, float y, color font_color
		) {
			auto text_surface = TTF_RenderText_Blended(
				font, text.c_str(), text.size(),font_color
			);
			if (!text_surface) return false;
			auto texture_w = text_surface->w;
			auto texture_h = text_surface->h;
			auto text_texture = SDL_CreateTextureFromSurface(
				m_renderer, text_surface);
			//doing this here so that the surface gets freed 
			//no matter what
			SDL_DestroySurface(text_surface);
			if (!text_texture) return false;
			auto rect = Frect(x - (texture_w / 2.0f), y - (texture_h / 2.0f), (float)texture_w, (float)texture_h); 
			SDL_RenderTexture(m_renderer, text_texture, &rect, NULL);

			SDL_DestroyTexture(text_texture);
			return true;
		}
		bool render_text(std::string text, TTF_Font* font, font_options options) {
			namespace rng = std::ranges;
			namespace views = std::ranges::views;
			
			std::vector<std::string> lines;

			if (TTF_GetFontSize(font) <= 0) return false; 
			if (text.empty())return false;
			

			//if we are told to not seperate any lines then we just render the text as a single line
			if(options.line_seperator == font_options::LINE_SEPERATOR_NONE) {
				//if the line separator is none then we just render the text as a single line
				if (options.text_alignment == options.TEXT_ALIGNMENT_CENTER) {
					return render_text_centered_simple(
						text, font, options.x, options.y, options.color
						
					);
				}
				return render_text_simple(
					text, font, options.x, options.y,
					options.color
				);
			} 
			//otherwise we split on the requested new line seq. 
			if (options.line_seperator == font_options::LINE_SEPARATOR_NEWLINE) {
				lines = rng::to<std::vector<std::string>>(rng::views::split(text, '\n')
					| views::transform([](auto&& str) { return std::string(str.begin(), str.end()); }));
			}
			if (options.line_seperator == font_options::LINE_SEPARATOR_CRLF) {
				lines = rng::to<std::vector<std::string>>(rng::views::split(text, "\r\n")
					| views::transform([](auto&& str) { return std::string(str.begin(), str.end()); }));
			}
			
			float line_y = 0; 
			SDL_Surface* text_surface = nullptr;
			for(const auto& line : lines) {
				auto text_line_surface = TTF_RenderText_Blended(
					font, line.c_str(), line.size(), options.color
				);
				if(!text_line_surface) {
					//TODO(skc) : handle error
					return false; 
				}
				if (text_line_surface->w <= 0 || text_line_surface->h <= 0 || !text_line_surface->format) {
					// skip empty/invalid line but advance line_y as appropriate
					if (options.line_height_mode == font_options::LINE_HEIGHT_MODE_ADDITIVE) {
						line_y += TTF_GetFontSize(font) + options.line_hight;
					}
					else {
						line_y += TTF_GetFontSize(font) * options.line_hight;
					}
					SDL_DestroySurface(text_line_surface);
					continue;
				}
				//NOTE(skc) : 
				// if this is the first line then we just set the text surface to the line surface
				//otherwise we align the line surface to the text surface based on the alignment option

				if(!text_surface) {
					text_surface = SDL_CreateSurface(text_line_surface->w, text_line_surface->h, text_line_surface->format);
					if (!text_surface) { return false; }
					SDL_BlitSurface(text_line_surface, NULL, text_surface, NULL);
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
			} //for(const auto& line : lines)
			if (!text_surface) return false;
			auto texture_w = text_surface->w;
			auto texture_h = text_surface->h;
			if (options.width != 0) texture_w = (int)options.width;
			if (options.height != 0) texture_h = (int)options.height;
			auto text_texture = SDL_CreateTextureFromSurface(m_renderer, text_surface);
			//NOTE(skc) : doing this here so that the surface gets freed
			//no matter what
			SDL_DestroySurface(text_surface);
			if (!text_texture) return false;
			Frect pos = {options.x, options.y, (float)texture_w, (float)texture_h };
			auto ap = options.anchor_point;
			//NOTE(skc) : I know that this is a code smell but this file is getting too big
#include "./private/position_from_anchor_point.hpp"
			//TODO(skc) : make this a settings option 
			SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
			//NOTE(skc) : SDL_TTF ingores the alpha channel so we have to set it manually?
			//(IDK why but alpha doesn't work unless we do this)
			SDL_SetTextureAlphaMod(text_texture, options.color.a); 
			auto rt = SDL_RenderTexture(m_renderer, text_texture, NULL, &pos);
			SDL_DestroyTexture(text_texture);
			return rt; 
		}
#pragma endregion
	};
}
//NOTE(skc): this is heere to scope this macro because it should not be used outside of this file.
#undef PRIV_GET_TEX_
#undef PRIV_PUSH_RENDER_BLEND_MODE_
#undef PRIV_POP_RENDER_BLEND_MODE_
#undef PRIV_SET_RENDER_BLEND_MODE_