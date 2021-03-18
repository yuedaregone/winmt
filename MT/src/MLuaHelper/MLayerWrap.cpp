#include "MLuaWrap.h"
#include "MLayer.h"
#include "MApplication.h"
#include "MTool.h"
#include "lextend.h"
extern "C"
{	
	static int newlayer(lua_State* L)
	{
		const char* name = luaL_checkstring(L, 1);
		int x = (int)luaL_checkinteger(L, 2);
		int y = (int)luaL_checkinteger(L, 3);
		int w = (int)luaL_checkinteger(L, 4);
		int h = (int)luaL_checkinteger(L, 5);

		MApplaction* app = MApplaction::GetCurrent();
		RECT rt = GetRect(x, y, w, h);

		MLayer** ml = (MLayer**)lua_newuserdata(L, sizeof(MLayer*));
		*ml = new MLayer(app->GetHInstance(), rt, name);
		int result = (*ml)->Create();
		if (result == 0)
		{
			app->Add((*ml));
		}
		else
		{
			delete (*ml);
		}

		luaL_getmetatable(L, "meta_mlayer");
		lua_setmetatable(L, -2);
		return 1;
	}

	static int deletelayer(lua_State* L) 
	{
		const char* name = luaL_checkstring(L, 1);
		luaL_argcheck(L, name != NULL, 1, "invalid call. You should set name!");

		MApplaction* app = MApplaction::GetCurrent();
		app->Destroy(name);
		return 0;
	}

	static int showlayer(lua_State* L)
	{
		MLayer** ml = (MLayer**)luaE_checkumetatable(L, 1, "meta_mlayer");
		luaL_argcheck(L, ml != NULL, 1, "invalid call. It should be MLayer!");

		if (*ml) (*ml)->Show(0);

		return 0;
	}

	static int renderlayer(lua_State* L)
	{
		MLayer** ml = (MLayer**)luaE_checkumetatable(L, 1, "meta_mlayer");
		luaL_argcheck(L, ml != NULL, 1, "invalid call. It should be MLayer!");

		if (*ml) (*ml)->Render();

		return 0;
	}

	static int setlayermanual(lua_State* L)
	{
		MLayer** ml = (MLayer**)luaE_checkumetatable(L, 1, "meta_mlayer");
		luaL_argcheck(L, ml != NULL, 1, "invalid call. It should be MLayer!");

		lua_Integer manual = luaL_checkinteger(L, 2);
		if (*ml) (*ml)->SetUpdateManual((bool)manual);

		return 0;
	}

	static int adddraw(lua_State* L)
	{
		MLayer** ml = (MLayer**)luaE_checkumetatable(L, 1, "meta_mlayer");
		luaL_argcheck(L, ml != NULL, 1, "invalid call. It should be MLayer!");

		void* draw = lua_touserdata(L, 2);
		if (draw != NULL)
		{
			MDrawProtocol* dinst = *((MDrawProtocol**)draw);
			(*ml)->AddDrawInst(dinst);
		}
		return 0;
	}

	static int rmvdraw(lua_State* L)
	{
		MLayer** ml = (MLayer**)luaE_checkumetatable(L, 1, "meta_mlayer");
		luaL_argcheck(L, ml != NULL, 1, "invalid call. It should be MLayer!");

		void* draw = lua_touserdata(L, 2);
		if (draw != NULL)
		{
			(*ml)->RemoveDrawInst((MDrawProtocol*)draw);
			return 0;
		}
		return 0;
	}

	static const luaL_Reg layerfunlib[] = {
		{ "show", showlayer },
		{ "render", renderlayer },
		{ "add", adddraw },
		{ "rmv", rmvdraw },
		{ NULL, NULL }
	};

	static const luaL_Reg layernewlib[] = {
		{"new", newlayer},
		{"del", deletelayer},
		{ NULL, NULL }
	};

	int luaopen_layer(lua_State *L) {
		luaL_newmetatable(L, "meta_mlayer");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, layerfunlib, 0);

		luaL_getmetatable(L, "meta_mwindow");
		lua_setmetatable(L, -2);

		luaL_newlib(L, layernewlib);
		lua_setglobal(L, "ML");
		return 1;
	}
}
