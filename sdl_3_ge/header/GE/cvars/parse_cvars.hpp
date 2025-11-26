#pragma once 
#include "CVar.hpp"
#include <format>
#include <string>

namespace SKC::GE {
	C_var_list parse_c_vars(int argc, char* argv[]) {
		C_var_list args;
        args.emplace_back("exe_path", std::string(argv[0]));
        //TODO(skc): This still needs work! 
         //should make this better 
        if (argc > 1) {
            std::string name{}, value{};
            for (int x = 1; x < argc; ++x) {
                std::string arg = argv[x];


                if ((arg.length() > 2 && (arg[0] == '-' && arg[1] == '-'))) {
                    //NOTE(skc): have to do this here to get the first arg
                    if (!name.empty()) {
                        if (!value.empty()) {
                            auto last_non_space = value.find_last_not_of(' ');
                            auto erase_len = value.length() - last_non_space;

                            if (erase_len) {
                                value = value.erase(last_non_space, erase_len);
                            }
                            args.emplace_back(name, value);
                        }
                        else {
                            args.emplace_back(name);
                        }
                    }

                    name = arg.erase(0, 2);
                    value = {};
                    continue;
                }
                else {
                    value += std::format("{} ", arg);
                }

                //NOTE(skc): and this here to get the last. 

                if (!value.empty()) {
                    args.emplace_back(name, value);
                }
                else {
                    args.emplace_back(name);
                }
            }
        }
        return args; 
	}
}