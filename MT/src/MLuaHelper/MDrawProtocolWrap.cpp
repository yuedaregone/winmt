#include <stdint.h>
#include "lextend.h"
#include "MDrawProtocol.h"
#include "MLuaWrap.h"

extern "C" {
	static int setpos(lua_State* L)
	{
		MDrawProtocol** draw = (MDrawProtocol**)luaE_checkumetatable(L, 1, "meta_draw");
		luaL_argcheck(L, draw != NULL, 1, "invalid call. It should be MDrawProtocol!");

		lua_Integer x = luaL_checkinteger(L, 2);
		lua_Integer y = luaL_checkinteger(L, 3);
		(*draw)->SetPosition((uint32_t)x, (uint32_t)y);
		return 0;
	}

	static int setscale(lua_State* L)
	{
		MDrawProtocol** draw = (MDrawProtocol**)luaE_checkumetatable(L, 1, "meta_draw");
		luaL_argcheck(L, draw != NULL, 1, "invalid call. It should be MDrawProtocol!");

		lua_Number x = luaL_checknumber(L, 2);
		lua_Number y = luaL_checknumber(L, 3);
		(*draw)->SetScale((float)x, (float)y);
		return 0;
	}

	static int setrotation(lua_State* L)
	{
		MDrawProtocol** draw = (MDrawProtocol**)luaE_checkumetatable(L, 1, "meta_draw");
		luaL_argcheck(L, draw != NULL, 1, "invalid call. It should be MDrawProtocol!");

		lua_Number r = luaL_checknumber(L, 2);
		(*draw)->SetRotation((float)r);
		return 0;
	}

	static int getdraww(lua_State* L)
	{
		MDrawProtocol** draw = (MDrawProtocol**)luaE_checkumetatable(L, 1, "meta_draw");
		luaL_argcheck(L, draw != NULL, 1, "invalid call. It should be MDrawProtocol!");

		lua_Integer w = (*draw)->GetW();
		lua_pushinteger(L, w);
		return 1;
	}

	static int getdrawh(lua_State* L)
	{
		MDrawProtocol** draw = (MDrawProtocol**)luaE_checkumetatable(L, 1, "meta_draw");
		luaL_argcheck(L, draw != NULL, 1, "invalid call. It should be MDrawProtocol!");

		lua_Integer h = (*draw)->GetH();
		lua_pushinteger(L, h);
		return 1;
	}	

	static int addcomp(lua_State* L)
	{
		MDrawProtocol** draw = (MDrawProtocol**)luaE_checkumetatable(L, 1, "meta_draw");
		luaL_argcheck(L, draw != NULL, 1, "invalid call. It should be MDrawProtocol!");

		MComponent** comp = (MComponent**)luaE_checkumetatable(L, 2, "meta_comp");
		luaL_argcheck(L, comp != NULL, 2, "invalid call. It should be MComponent!");

		(*draw)->AddComponent((*comp));
		return 0;
	}

	static int rmvcomp(lua_State* L)
	{
		MDrawProtocol** draw = (MDrawProtocol**)luaE_checkumetatable(L, 1, "meta_draw");
		luaL_argcheck(L, draw != NULL, 1, "invalid call. It should be MDrawProtocol!");

		MComponent** comp = (MComponent**)luaE_checkumetatable(L, 2, "meta_comp");
		luaL_argcheck(L, comp != NULL, 2, "invalid call. It should be MComponent!");

		(*draw)->RemoveComponent((*comp));
		return 0;
	}

	static const luaL_Reg labelfunlib[] = {
		{ "setpos", setpos },
		{ "setscale", setscale },
		{ "setrotation", setrotation },
		{ "getw", getdraww },
		{ "geth", getdrawh },
		{ "add", addcomp },
		{ "remove", rmvcomp},
		{ NULL, NULL }
	};

	int luaopen_mdraw(lua_State* L) {
		luaL_newmetatable(L, "meta_draw");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, labelfunlib, 0);
		return 1;
	}

}