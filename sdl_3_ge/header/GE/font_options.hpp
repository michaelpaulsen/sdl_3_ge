#pragma once
#include "Color.hpp"
namespace SKC::GE {

	struct font_options {
		float line_hight{ 1 }, x, y, positionoffset_x, positionoffset_y;
		unsigned int width, height;
		color color;

		enum line_hight_mode : uint8_t {
			LINE_HEIGHT_MODE_ADDITIVE = 0, //fixed line height
			LINE_HEIGHT_MODE_MULTIPLICATIVE, //multiplier of font size
		} line_height_mode{ LINE_HEIGHT_MODE_ADDITIVE };
		enum text_alignment : uint8_t {
			TEXT_ALIGNMENT_LEFT = 0, //left aligned
			TEXT_ALIGNMENT_CENTER,   //centered
			TEXT_ALIGNMENT_RIGHT,    //right aligned
		} text_alignment{ TEXT_ALIGNMENT_LEFT };
		enum line_alignment : uint8_t {
			LINE_ALIGNMENT_TOP = 0, //top aligned
			LINE_ALIGNMENT_CENTER,  //centered
			LINE_ALIGNMENT_BOTTOM,  //bottom aligned
		} line_alignment{ LINE_ALIGNMENT_TOP };
		enum line_seperator : uint8_t {
			LINE_SEPERATOR_NONE = 0,  //any separator
			LINE_SEPARATOR_NEWLINE,  //newline separator
			LINE_SEPARATOR_CRLF,     //space separator
		} line_separator{ LINE_SEPERATOR_NONE };
		enum anchor_point : uint8_t {
			AP_FLOAT = 0, 
			AP_TOP_LEFT,
			AP_CENTER_LEFT, 
			AP_BOTTOM_LEFT,
			AP_TOP_RIGHT,
			AP_CENTER_RIGHT, 
			AP_BOTTOM_RIGHT,
			AP_CENTER,
		} anchor_point{ AP_TOP_LEFT };
	};
}