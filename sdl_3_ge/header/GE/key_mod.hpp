namespace SKC::GE {
	struct key_mod {
		bool lshift,
			rshift,
			rctrl,
			lctrl,
			lalt,
			ralt,
			lgui,
			rgui,
			num,
			caps,
			scroll;
		explicit key_mod() :
			lshift(false),
			rshift(false),
			rctrl(false),
			lctrl(false),
			ralt(false),
			lalt(false),
			rgui(false),
			lgui(false),
			num(false),
			caps(false),
			scroll(false)
		{
		}

		key_mod(const key_mod& other) noexcept {
			lshift = other.lshift;
			rshift = other.rshift;
			rctrl = other.rctrl;
			lctrl = other.lctrl;
			ralt = other.ralt;
			lalt = other.lalt;
			rgui = other.rgui;
			lgui = other.lgui;
			num = other.num;
			caps = other.caps;
			scroll = other.scroll;
		}
		key_mod(key_mod&& other) noexcept {
			lshift = other.lshift;
			rshift = other.rshift;
			rctrl = other.rctrl;
			lctrl = other.lctrl;
			ralt = other.ralt;
			lalt = other.lalt;
			rgui = other.rgui;
			lgui = other.lgui;
			num = other.num;
			caps = other.caps;
			scroll = other.scroll;
			other = key_mod(0);
		}

		void operator=(const key_mod& other) noexcept {
			lshift = other.lshift;
			rshift = other.rshift;
			rctrl = other.rctrl;
			lctrl = other.lctrl;
			ralt = other.ralt;
			lalt = other.lalt;
			rgui = other.rgui;
			lgui = other.lgui;
			num = other.num;
			caps = other.caps;
			scroll = other.scroll;
		}
		explicit key_mod(uint16_t keymod) :
			lshift(keymod& SDL_KMOD_LSHIFT),
			rshift(keymod& SDL_KMOD_RSHIFT),
			rctrl(keymod& SDL_KMOD_RCTRL),
			lctrl(keymod& SDL_KMOD_LCTRL),
			ralt(keymod& SDL_KMOD_RALT),
			lalt(keymod& SDL_KMOD_LALT),
			rgui(keymod& SDL_KMOD_RGUI),
			lgui(keymod& SDL_KMOD_LGUI),
			num(keymod& SDL_KMOD_NUM),
			caps(keymod& SDL_KMOD_CAPS),
			scroll(keymod& SDL_KMOD_SCROLL)
		{
		}

		~key_mod() = default;

		bool shift() const noexcept {
			return rshift || lshift;
		}
		bool alt() const noexcept {
			return ralt || lalt;
		}
		bool gui() const noexcept {
			return rgui || lgui;

		}
		bool ctrl() const noexcept {
			return rctrl || lctrl;

		}
		//TODO(skc) : implement -> SDL_KMOD_LEVEL5



	};
}

//TODO(SKC) : make this so that it only outputs in dbg mode... 

//NOTE(SKC): FORMAT_STRING_GET is assumed to be undefed at the end of this function  
// things may break if you remove the #undef FORMAT_STRING_GET. 
#define FORMAT_STRING_GET(prop, val) string += val; \
	if (obj.prop) {string += " true ";} \
	else {string += " false";} \
	if(new_line)string += '\n';\
	else string += ' ';
template <>
struct std::formatter<SKC::GE::key_mod> {
	bool new_line = false;

	constexpr auto parse(std::format_parse_context& ctx) {
		auto pos = ctx.begin();
		while (pos != ctx.end() && *pos != '}') {
			if (*pos == 'n')
				new_line = true;
			++pos;
		}
		return pos;
		//FIXME(skc): this should throw if *pos != '}' 
		//because this should error when it is not. 
		//however this is not an huge issue...

	}
	auto format(const SKC::GE::key_mod& obj, std::format_context& ctx) const {
		auto string = std::string();
		if (new_line) {
			string += "===KEY MODIFIER STATE===\n";
		}
		else {
			string += "Key State ->";

		}

		FORMAT_STRING_GET(lshift, "LSFT ");
		FORMAT_STRING_GET(rshift, "RSFT ");
		FORMAT_STRING_GET(lctrl, "RCTRL ");
		FORMAT_STRING_GET(rctrl, "LCTRL ");
		FORMAT_STRING_GET(lalt, "LALT ");
		FORMAT_STRING_GET(ralt, "LALT ");
		FORMAT_STRING_GET(lgui, "LMEN ");
		FORMAT_STRING_GET(rgui, "RMEN ");
		FORMAT_STRING_GET(num, "NUMLOK ");
		FORMAT_STRING_GET(caps, "CAPSLOK ");
		FORMAT_STRING_GET(scroll, "SCRLLOK ");
		return std::format_to(ctx.out(), "{}", string);
	}

};
#undef FORMAT_STRING_GET