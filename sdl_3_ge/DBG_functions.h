#pragma once
#ifdef _DEBUG
#define DBG_ASSERT(cnd) if(!cnd) _CrtDbgBreak() 
#else
#define DBG_ASSERT(cnd)
#endif
