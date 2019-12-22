#include <stdint.h>
#include "MSprite.h"
#include "MLuaWrap.h"

extern "C" {
	static int newspf(lua_State* L)
	{
		const char* filename = luaL_checkstring(L, 1);		
		MSprite** lb = (MSprite**)lua_newuserdata(L, sizeof(MSprite*));
		*lb = MSprite::CreateSprite(filename);

		luaL_getmetatable(L, "meta_sprite");
		lua_setmetatable(L, -2);
		return 1;
	}

	static int newspb(lua_State* L)
	{
		const char* filename = luaL_checkstring(L, 1);
		MSprite** lb = (MSprite**)lua_newuserdata(L, sizeof(MSprite*));
		*lb = MSprite::CreateBuiltinSprite(filename);

		luaL_getmetatable(L, "meta_sprite");
		lua_setmetatable(L, -2);
		return 1;
	}

	static int deletesp(lua_State* L)
	{
		MSprite** sp = (MSprite**)luaL_checkudata(L, 1, "meta_sprite");
		luaL_argcheck(L, sp != NULL, 1, "invalid call. It should be MSprite!");

		delete (*sp);
		return 0;
	}

	static const luaL_Reg newspritelib[] = {
		{ "new", newspf },
		{ "new_t", newspb },
		{ NULL, NULL }
	};

	static const luaL_Reg spritefunlib[] = {
		{ "release", deletesp },
		{ NULL, NULL }
	};

	int luaopen_msprite(lua_State* L) {
		luaL_newmetatable(L, "meta_sprite");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, spritefunlib, 0);

		luaL_getmetatable(L, "meta_draw");
		lua_setmetatable(L, -2);

		luaL_newlib(L, newspritelib);
		lua_setglobal(L, "Sprite");
		return 1;
	}
}