#include "MLuaWrap.h"
#include "MApplication.h"
#include "MTool.h"
#include "MWindow.h"
#include "lextend.h"
extern "C" {	
	static int newmw(lua_State* L)
	{
		const char* name = luaL_checkstring(L, 1);
		int x = (int)luaL_checkinteger(L, 2);
		int y = (int)luaL_checkinteger(L, 3);
		int w = (int)luaL_checkinteger(L, 4);
		int h = (int)luaL_checkinteger(L, 5);

		MApplaction* app = MApplaction::GetCurrent();
		RECT rt = GetRect(x, y, w, h);

		MWindow** mw = (MWindow**)lua_newuserdata(L, sizeof(MWindow*));
		*mw = new MWindow(app->GetHInstance(), rt, name);		
		int result = (*mw)->Create();
		if (result == 0)
		{
			app->Add((*mw));
		}
		else
		{
			delete (*mw);
		}

		luaL_getmetatable(L, "meta_mwindow");
		lua_setmetatable(L, -2);
		return 1;
	}

	static int showmw(lua_State* L)
	{
		MWindow** mw = (MWindow**)luaE_checkumetatable(L, 1, "meta_mwindow");
		luaL_argcheck(L, mw != NULL, 1, "invalid call. It should be MWindow!");
		
		int show = (int)luaL_checknumber(L, 2);

		if (*mw && !(*mw)->IsEnd()) 
			(*mw)->Show(show);

		return 0;
	}

	static int endmw(lua_State* L)
	{
		MWindow** mw = (MWindow**)luaE_checkumetatable(L, 1, "meta_mwindow");
		luaL_argcheck(L, mw != NULL, 1, "invalid call. It should be MWindow!");

		if (*mw && !(*mw)->IsEnd()) 
			(*mw)->End();

		return 0;
	}

	static int getww(lua_State* L)
	{
		MWindow** mw = (MWindow**)luaE_checkumetatable(L, 1, "meta_mwindow");
		luaL_argcheck(L, mw != NULL, 1, "invalid call. It should be MWindow!");

		unsigned int w = 0;
		if (*mw && !(*mw)->IsEnd()) 
			w = (*mw)->GetWidth();

		lua_pushnumber(L, (lua_Number)w);
		return 1;
	}

	static int getwh(lua_State* L)
	{
		MWindow** mw = (MWindow**)luaE_checkumetatable(L, 1, "meta_mwindow");
		luaL_argcheck(L, mw != NULL, 1, "invalid call. It should be MWindow!");

		unsigned int h = 0;
		if (*mw && !(*mw)->IsEnd())
			h = (*mw)->GetHeight();

		lua_pushnumber(L, (lua_Number)h);
		return 1;
	}

	static int getname(lua_State* L)
	{
		MWindow** mw = (MWindow**)luaE_checkumetatable(L, 1, "meta_mwindow");
		luaL_argcheck(L, mw != NULL, 1, "invalid call. It should be MWindow!");

		const char* name = "Not found";
		if (*mw && !(*mw)->IsEnd())
			name = (*mw)->GetNameCStr();

		lua_pushstring(L, name);
		return 1;
	}

	static const luaL_Reg wmnewlib[] = {
		{ "new", newmw },
		{ NULL, NULL }
	};

	static const luaL_Reg wmfunlib[] = {
		{ "show", showmw },
		{ "stop", endmw },
		{ "width", getww },
		{ "height", getwh },

		{ NULL, NULL }
	};
	
	int luaopen_mwindow(lua_State* L) {
		luaL_newmetatable(L, "meta_mwindow");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, wmfunlib, 0);

		luaL_newlib(L, wmnewlib);
		lua_setglobal(L, "MW");
		return 1;
	}
}