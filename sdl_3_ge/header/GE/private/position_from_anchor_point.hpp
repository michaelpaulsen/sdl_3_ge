#pragma once
#include "../font_options.hpp"
#include <SDL3/SDL_rect.h>
namespace SKC::GE::PRIVATE {




	SDL_FRect offset_anchor(font_options options, int texture_w, int texture_h) {
		
		SDL_FRect pos = { options.x, options.y, (float)texture_w, (float)texture_h };

		
		//NOTE(skc) : 
		//   anchor point translation
		//---------------------------
		//    AP_FLOATING (w*ox, h*oy)
		//	  AP_TOP_LEFT ( 0  ,  0  )
		// AP_CENTER_LEFT ( 0  , -h/2)
		// AP_BOTTOM_LEFT ( 0  , -h  ) 
		//   AP_TOP_RIGHT (-w  ,  0  )
		//AP_CENTER_RIGHT (-w  , -h/2)
		//AP_BOTTOM_RIGHT (-w  , -h  )
		//      AP_CENTER (-w/2, -h/2)
		auto ap = options.anchor_point; 
		switch (ap) {
		case font_options::AP_FLOAT:
			pos.x += options.positionoffset_x * texture_w;
			pos.y += options.positionoffset_y * texture_h;
			return pos;
		case font_options::AP_CENTER_LEFT:
			pos.y -= (float)(texture_h) / 2.0f;
			return pos;
		case font_options::AP_TOP_LEFT:
			break;
		case font_options::AP_BOTTOM_LEFT:
			pos.y -= (float)(texture_h);
			return pos;
		case font_options::AP_TOP_RIGHT:
			pos.x -= (float)(texture_w);
			return pos;
		case font_options::AP_CENTER_RIGHT:
			pos.x -= (float)(texture_w);
			pos.y -= (float)(texture_h) / 2.0f;
			return pos;
		case font_options::AP_BOTTOM_RIGHT:
			pos.x -= (float)(texture_w);
			pos.y -= (float)(texture_h);
			return pos;
		case font_options::AP_CENTER:
			pos.x -= (float)(texture_w) / 2.0f;
			pos.y -= (float)(texture_h) / 2.0f;
			return pos;
		}
		return pos; 
	}
}