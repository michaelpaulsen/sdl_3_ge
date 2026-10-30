#pragma once
namespace skc::commands {
	void create_command() {
		auto subcommand = command.consume();
		if (subcommand == "texture") {
			auto name = command.consume();
			long long tid = 0;

			int x = window_size.x, y = window_size.y;
			switch (command.args()) {
			case 1: {
				tid = window.create_texture();
				break;
			}
			case 2: {
				x = skc::util::to_number<int>(command.consume());
				y = skc::util::to_number<int>(command.consume());
				tid = window.create_texture(x, y);
				break;

			}
			default: return true;
			}

			{
				auto lg = std::lock_guard(lua);
				lua.get_global(texture_array_name);
				lua.push(name);
				lua.create_table();
				lua.append_table("tid", tid);
				lua.append_table("width", (long long)x);
				lua.append_table("height", (long long)y);
				lua.append_table();
				lua.set_global(texture_array_name);

			}
			return true;
		}


	}

}