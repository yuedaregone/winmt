#ifndef __LEXTEND_H__
#define __LEXTEND_H__
#include "lua.hpp"

extern "C" {
	void* luaE_checkumetatable(lua_State* L, int ud, const char* tname);
}
#endif
