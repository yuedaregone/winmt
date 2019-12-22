#include <stdint.h>
#include "lextend.h"
#include "MComponent.h"
#include "MLuaWrap.h"
#include "MLuaEngine.h"

class MComponentWrap : public MComponent
{
public:
	MComponentWrap() : m_function(LUA_REFNIL) {}

	virtual void OnUpdate()
	{
		if (m_function != LUA_REFNIL)
		{
			lua_State* L = g_mLuaEngine->GetL();
			lua_rawgeti(L, LUA_REGISTRYINDEX, m_function);			
			lua_pcall(L, 0, 0, 0);
		}		
	}

	void RegisterUpdate(int fun)
	{
		if (m_function != LUA_REFNIL)
		{			
			g_mLuaEngine->ReleaseCacheFunction(m_function);			
		}
		m_function = fun;
	}

	void UngisterUpdate()
	{
		if (m_function != LUA_REFNIL)
		{
			g_mLuaEngine->ReleaseCacheFunction(m_function);
			m_function = LUA_REFNIL;
		}
	}

private:
	int m_function;
};

extern "C" {
	static int newcomp(lua_State* L)
	{
		MComponentWrap** comp = (MComponentWrap**)lua_newuserdata(L, 1);
		*comp = new MComponentWrap();

		luaL_getmetatable(L, "meta_comp");
		lua_setmetatable(L, -2);
		return 1;
	}

	static int regcb(lua_State* L)
	{
		MComponentWrap** comp = (MComponentWrap**)luaE_checkumetatable(L, 1, "meta_comp");
		luaL_argcheck(L, comp != NULL, 1, "invalid call. It should be MComponentWrap!");

		lua_pushvalue(L, 2);
		int func = luaL_ref(L, LUA_REGISTRYINDEX);
		lua_pop(L, 1);

		(*comp)->RegisterUpdate(func);		
		return 0;
	}

	static int unregcb(lua_State* L)
	{
		MComponentWrap** comp = (MComponentWrap**)luaE_checkumetatable(L, 1, "meta_comp");
		luaL_argcheck(L, comp != NULL, 1, "invalid call. It should be MComponentWrap!");
				
		(*comp)->UngisterUpdate();
		return 0;
	}	

	static const luaL_Reg compfunlib[] = {
		{ "reg", regcb },
		{ "unreg", unregcb },
		{ NULL, NULL }
	};

	static const luaL_Reg newfunlib[] = {
		{ "new", newcomp },
		{ NULL, NULL }
	};

	int luaopen_mcomp(lua_State* L) {
		luaL_newmetatable(L, "meta_comp");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, compfunlib, 0);
			

		luaL_newlib(L, newfunlib);
		lua_setglobal(L, "COMP");
		return 1;
	}

}