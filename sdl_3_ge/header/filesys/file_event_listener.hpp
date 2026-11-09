#pragma once
#ifdef _WIN32
#include "./windows/folder_listener.hpp"
#endif // _WIN32
#ifdef LINUX 
#error linux not supported
//#include "./linux/folder_listener.hpp"
#endif
