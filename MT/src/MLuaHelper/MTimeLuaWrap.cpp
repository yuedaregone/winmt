#include "MLuaWrap.h"
#include "MTime.h"
extern "C" {

	static int time(lua_State* L)
	{
		float t = MTime::time();
		lua_pushnumber(L, t);
		return 1;
	}

	static int delay(lua_State* L)
	{
		float t = MTime::delay();
		lua_pushnumber(L, t);
		return 1;
	}

	static int rtime(lua_State* L)
	{
		float t = MTime::rtime();
		lua_pushnumber(L, t);
		return 1;
	}

	static int rdelay(lua_State* L)
	{
		float t = MTime::rdelay();
		lua_pushnumber(L, t);
		return 1;
	}

	static int scale(lua_State* L)
	{
		float t = MTime::scale();
		lua_pushnumber(L, t);
		return 1;
	}

	static int sscale(lua_State* L)
	{
		lua_Number s = luaL_checknumber(L, 1);
		MTime::sscale((float)s);		
		return 0;
	}

	static const luaL_Reg timelib[] = {
		{ "time", time },
		{ "delay", delay },
		{ "rtime", rtime },
		{ "rdelay", rdelay },
		{ "scale", scale },
		{ "sscale", sscale },
		{ NULL, NULL }
	};

	int luaopen_time(lua_State *L) {
		luaL_newlib(L, timelib);
		lua_setglobal(L, "mtime");
		return 1;
	}
}
