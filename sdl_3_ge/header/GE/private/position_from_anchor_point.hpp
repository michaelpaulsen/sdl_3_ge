
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

switch (ap) {
case font_options::AP_FLOAT:
	pos.x += options.positionoffset_x * texture_w;
	pos.y += options.positionoffset_y * texture_h;
	break;
case font_options::AP_CENTER_LEFT:
	pos.y -= (float)(texture_h) / 2.0f;
	break;
case font_options::AP_TOP_LEFT:
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