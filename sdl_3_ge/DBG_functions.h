#pragma once
#ifdef _DEBUG
#define DBG_ASSERT(cnd) if(!cnd) _CrtDbgBreak() 
#else
#define assert(cnd)
#endif
