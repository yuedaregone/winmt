#pragma once
#include "lua.hpp"
extern "C" {
	int luaopen_time(lua_State *L);
	int luaopen_keyevent(lua_State *L);
	int luaopen_mwindow(lua_State *L);
	int luaopen_layer(lua_State *L);
	int luaopen_http(lua_State *L);
	int luaopen_mlabel(lua_State *L);
	int luaopen_msprite(lua_State *L);
	int luaopen_win(lua_State *L);

	int luaopen_mdraw(lua_State *L);
	int luaopen_mcomp(lua_State *L);

	inline int luaopen_mall(lua_State *L)
	{
		luaopen_time(L);
		luaopen_keyevent(L);
		luaopen_mwindow(L);
		luaopen_layer(L);
		luaopen_http(L);
		luaopen_win(L);

		luaopen_mdraw(L);
		luaopen_mcomp(L);
		luaopen_mlabel(L);
		luaopen_msprite(L);
		
		return 0;
	}
}



