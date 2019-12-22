#include <stdint.h>
#include "MLabel.h"
#include "MLuaWrap.h"

extern "C" {
	static int newlabel(lua_State* L)
	{
		int n = lua_gettop(L);
		const char* content = luaL_checkstring(L, 1);
		uint32_t fontSize = (uint32_t)luaL_checkinteger(L, 2);
		if (fontSize == 0 || fontSize > 64) fontSize = 16;

		MLable** lb = (MLable**)lua_newuserdata(L, sizeof(MLable*));
		*lb = MLable::CreateLabel(content, fontSize);

		luaL_getmetatable(L, "meta_label");
		lua_setmetatable(L, -2);
		return 1;
	}

	static int setcontent(lua_State* L)
	{
		MLable** label = (MLable**)luaL_checkudata(L, 1, "meta_label");
		luaL_argcheck(L, label != NULL, 1, "invalid call. It should be MLable!");

		const char* ctn = luaL_checkstring(L, 2);
		(*label)->SetContent(ctn);
		return 0;
	}

	static int dellabel(lua_State* L)
	{
		MLable** label = (MLable**)luaL_checkudata(L, 1, "meta_label");
		luaL_argcheck(L, label != NULL, 1, "invalid call. It should be MLable!");

		delete (*label);
		return 0;
	}

	static const luaL_Reg newlabellib[] = {
		{ "new", newlabel },
		{ NULL, NULL }
	};

	static const luaL_Reg labelfunlib[] = {
		{ "set", setcontent },
		{ "release", dellabel },
		{ NULL, NULL }
	};

	int luaopen_mlabel(lua_State* L) {
		luaL_newmetatable(L, "meta_label");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, labelfunlib, 0);

		luaL_getmetatable(L, "meta_draw");
		lua_setmetatable(L, -2);

		luaL_newlib(L, newlabellib);
		lua_setglobal(L, "Label");
		return 1;
	}

}