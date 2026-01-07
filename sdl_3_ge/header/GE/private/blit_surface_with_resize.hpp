SDL_Surface* blit_surface_with_resize(SDL_Surface* dst, SDL_Surface* src, int x, int y) {
	if (!dst || !src) return dst;
	if (!src->w || !src->h) return dst;
	int needed_width = x + src->w;
	int needed_height = y + src->h;
	//if we don't need to resize the surface then we should just use it.
	SKC::GE::rect src_rect = { x,y,src->w,src->h };
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
	if (needed_width < dst->w) needed_width = dst->w;
	if (!x_is_positive) { needed_width += -x; }
	//if the y coordinate is negative then we need to add the height of the source surface to the destination surface height
	if (!y_is_positive) {
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