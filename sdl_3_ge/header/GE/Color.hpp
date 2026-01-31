#pragma once

#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_pixels.h>
#include <algorithm>
#include <cmath>
namespace SKC::GE {
	using c_t = Uint8;
	
	struct color_hsl; 

	struct color {

		c_t r, g, b, a;
		color() = default;
		explicit color(c_t w) : r(w), g(w), b(w), a(255) {}
		color(c_t r, c_t g, c_t b, c_t a = 255) : r(r), g(g), b(b), a(a) {}

		// TODO (skc) : implement a CSS color name constructor...
		//there's like a lot of them though so that might have to be itsown file... 
		color operator +(color other) {
			return color(r + other.r, g + other.g, b + other.b );
		}
		operator color_hsl();
		operator SDL_Color() const {
			return SDL_Color{ r,g,b,a };
		}
	};

	struct color_hsl {
		using T = double; 
		T h, s, l;
		c_t a; 
		
		color_hsl() = default;
		~color_hsl() = default;
		color_hsl(const color_hsl& c) = default;
		color_hsl(color_hsl&& c) = default;
		
		
		explicit color_hsl(T hv) : h(hv), s(1), l(1),a(255) {};
		color_hsl(T hv, T sv, T  lv = 1, c_t av = 255)
			: s(sv), l(lv), a(av) {
			h = fmod(hv,360); 
		};
		
		
		color_hsl& operator= (const color_hsl& c) = default;
		color_hsl& operator= ( color_hsl&& c) = default;
		
		operator color() {
			if (s == 0) {
				auto c = color(c_t(l * 255));
				c.a = a; 
				return c;
			}
			double q = l * (1 + s),
				p = 2 * l;
			if (l > .5) {
				q= l + s - l * s;
			}
			p -= q; 
			auto tdiff = q - p;
			double th = h / 360; 
			double tr = th + .333; 
			double tg = th; 
			double tb = th - .333; 		
			
			auto hue2rgb = [&](double t) mutable {
				if (t < 0) t += 1;
				if (t > 1) t -= 1;

				if (6 * t < 1) { return p + tdiff * 6 * t; }
				if (2 * t < 1) { return q; }
				if (3 * t < 2) { return p + tdiff * (2.0 / 3.0 - t) * 6; }
				return p;
			};
			return { c_t(hue2rgb(tr) * 255),c_t(hue2rgb(tg) * 255),c_t(hue2rgb(tb) * 255), a };
		}
	
	};


	color::operator color_hsl() {
		double fr = (double)r / 255,
			fg = (double)g / 255,
			fb = (double)b / 255;
		double max = std::max(fr, std::max(fg, fb));
		double min = std::min(fr, std::min(fg, fb));
		auto range = max - min;

		double l = (max + min) / 2, s = 0.f, h = 0.f;
		if (max == min) {
			return { 0.,0.,max };
		}
		if (l <= .5) {
			s = (range) / (max + min);
		}
		if (not (max == min) and l > .5) {
			s = (range) / (2.0 - max + min);
		}
		if (fr == max) {
			h = (fg - fb) / range;
		}
		if (fg == max) {
			h = 2. + (fb - fr) / range;
		}
		if (fb == max) {
			h = 4.0 + (fr - fg) / range;
		}
		h *= 60;
		if (h < 0) h += 360;

		return { h,s,l };
	};
	//TODO(skc): move to own file and include functional? 
	color interperlate_colorRGB(auto bais_func, color color1, color color2, double weight) {
		if (weight >= 1) return color2; 
		if (weight <= 0) return color1;
		auto w1 = bais_func(weight); 
		auto w2 = bais_func(1 - weight); 
		color cr{};
		cr.r = (color1.r * w1) + (color2.r * w2); 
		cr.g = (color1.g * w1) + (color2.g * w2); 
		cr.b = (color1.b * w1) + (color2.b * w2); 
		return cr; 
	}
}

#define            SKCC_RED COLOR_NAMESPACE::color(0x44, 0x00, 0x00)
#define           SKCC_BLUE COLOR_NAMESPACE::color(0x00, 0x00, 0x44)
#define           SKCC_GOLD COLOR_NAMESPACE::color(0xff, 0xd7, 0x00)
#define          SKCC_AZURE COLOR_NAMESPACE::color(0xf0, 0xff, 0xff)
#define          SKCC_BLACK COLOR_NAMESPACE::color(0x00, 0x00, 0x00)
#define          SKCC_GREEN COLOR_NAMESPACE::color(0x00, 0x44, 0x00)
#define         SKCC_ORANGE COLOR_NAMESPACE::color(0xFF, 0xA5, 0x00)
#define         SKCC_YELLOW COLOR_NAMESPACE::color(0xff, 0xff, 0x00)
#define       SKCC_MID_GREY COLOR_NAMESPACE::color(128) 
#define     SKCC_BRIGHT_RED COLOR_NAMESPACE::color(0xff, 0x00, 0x00)
#define     SKCC_GOLDEN_ROD COLOR_NAMESPACE::color(0xDA, 0xA5, 0x20)
#define    SKCC_BRIGHT_BLUE COLOR_NAMESPACE::color(0x00, 0x00, 0xff)
#define   SKCC_BRIGHT_GREEN COLOR_NAMESPACE::color(0x00, 0xff, 0x00)
#define   SKCC_NAVAJO_WHITE COLOR_NAMESPACE::color(0xFF, 0xDE, 0xAD)
#define SKCC_DARK_GOLDENROD COLOR_NAMESPACE::color(0xb8, 0x86, 0x0b)