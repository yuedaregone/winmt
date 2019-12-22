#include "lextend.h"
extern "C" {
	static void* luaE_testudata(lua_State* L, int ud, const char* tname) {
		void *p = lua_touserdata(L, ud);		
		if (p != NULL) {
			void *r = NULL;
			luaL_getmetatable(L, tname);
			int midx = lua_gettop(L);
			int cidx = ud;
			while (lua_getmetatable(L, cidx))
			{
				if (lua_rawequal(L, -1, midx))
				{
					r = p;
					break;
				}						
				cidx = -1;
			}
			lua_settop(L, midx - 1);
			return r;
		}
		return NULL;
	}

	void* luaE_checkumetatable(lua_State* L, int ud, const char* tname)
	{
		void* p = luaE_testudata(L, ud, tname);
		if (p == NULL) luaL_error(L, "arg error: should be tname");
		return p;
	}
}



