#include "MLuaWrap.h"
#include "MKeyboardListen.h"
#include "MLuaEngine.h"

extern "C" {
	static bool OnKeyEvent(int state, int code)
	{		
		lua_State* L = g_mLuaEngine->GetL();
		if (lua_getglobal(L, "keyevent") != LUA_TTABLE)
			return false;

		if (lua_getfield(L, -1, "events") != LUA_TTABLE)
 			return false;

		int ret = 0;
		int index = lua_gettop(L);
		lua_pushnil(L);
		while (lua_next(L, index) != 0)
		{
			if (lua_type(L, -1) == LUA_TFUNCTION)
			{
 				lua_pushnumber(L, state);
				lua_pushnumber(L, code);
				lua_pcall(L, 2, 1, 0);
				int t = lua_toboolean(L, -1);
				ret |= t;
			}
			lua_pop(L, 1);
		}
		return (bool)ret;
	}

	static int startListenKeyEvent(lua_State* L)
	{
		g_mKeyboardListen->RegisterKeyListen(OnKeyEvent);
		return 0;
	}

	static int stopListenKeyEvent(lua_State* L)
	{
		g_mKeyboardListen->UnregisterKeyListen(OnKeyEvent);
		return 0;
	}

	static int clickingKey(lua_State* L)
	{
		int keycode = (int)luaL_checkinteger(L, 1);
		bool click = g_mKeyboardListen->IsClickKey(keycode);
		lua_pushboolean(L, click);
		return 1;
	}

	static const luaL_Reg keyeventlib[] = {
		{"start", startListenKeyEvent },
		{"stop", stopListenKeyEvent },
		{"clicked", clickingKey },
		{NULL, NULL}
	};

	int luaopen_keyevent(lua_State *L) {
		luaL_newlib(L, keyeventlib);
		lua_createtable(L, 0, 0);
		lua_setfield(L, -2, "events");
		lua_setglobal(L, "keyevent");
		return 1;
	}

}
