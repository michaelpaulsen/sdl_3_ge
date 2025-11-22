#pragma once

#include <SDL3/SDL.h>
#include <vector>

namespace SKC::GE {
	using c_t = Uint8;
	struct color {
		
		c_t r, g, b, a;
		color() = default; 
		explicit color(c_t w) : r(w), g(w), b(w),a(255) {}
		color(c_t r, c_t g, c_t b, c_t a = 255) : r(r),g(g),b(b), a(a){ }
		// TODO (skc) : implement a CSS color name constructor...
		//there's like a lot of them though so that might have to be itsown file... 

	};
	
}
