#pragma once
#ifdef _DEBUG
#define assert(cnd) if(!cnd) _CrtDbgBreak() 
#else
#define assert(cnd)
#endif
