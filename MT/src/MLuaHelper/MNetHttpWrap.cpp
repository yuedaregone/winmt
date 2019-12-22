#include "MLuaWrap.h"
#include "MHttpClient.h"
#include "MLuaEngine.h"
#include "MHttpResponse.h"

class MHttpClientLuaWrap : public MHttpClient
{
public:
	MHttpClientLuaWrap() 
		: m_funCallback(LUA_REFNIL)
	{
	}

	~MHttpClientLuaWrap()
	{		
		g_mLuaEngine->ReleaseCacheFunction(m_funCallback);		
	}

	void RegisterLuaCallback(int func)
	{
		if (m_funCallback != LUA_REFNIL)
		{
			g_mLuaEngine->ReleaseCacheFunction(m_funCallback);
		}
		m_funCallback = func;
	}

protected:
	virtual void OnRecvEnd()
	{
		if (m_funCallback != LUA_REFNIL)
		{
			const char* data = (const char*)m_response->GetDataPtr();
			size_t len = m_response->GetContentSize();

			lua_State* L = g_mLuaEngine->GetL();	
			lua_rawgeti(L, LUA_REGISTRYINDEX, m_funCallback);

			lua_pushlstring(L, data, len);
			lua_pushinteger(L, (lua_Integer)len);
			lua_pcall(L, 2, 0, 0);
		}	
		m_finished = false;
		printf("Finish\n");
		m_socket->CloseSocket();
	}

	int m_funCallback;
};

extern "C" {
	static int newclient(lua_State* L)
	{
		MHttpClientLuaWrap** client = (MHttpClientLuaWrap**)lua_newuserdata(L, sizeof(MHttpClientLuaWrap*));
		*client = new MHttpClientLuaWrap();

		luaL_getmetatable(L, "meta_http");
		lua_setmetatable(L, -2);
		return 1;
	}

	static int reghttp(lua_State* L)
	{
		int n = lua_gettop(L);
		
		MHttpClientLuaWrap** client = (MHttpClientLuaWrap**)luaL_checkudata(L, 1, "meta_http");
		luaL_argcheck(L, client != NULL, 1, "invalid call. It should be MHttpClientLuaWrap!");

		if (n > 1)
		{
			lua_pushvalue(L, 2);
			int func = luaL_ref(L, LUA_REGISTRYINDEX);
			lua_pop(L, 1);
			(*client)->RegisterLuaCallback(func);
		}		
		return 0;
	}

	static int request(lua_State* L)
	{
		MHttpClientLuaWrap** client = (MHttpClientLuaWrap**)luaL_checkudata(L, 1, "meta_http");
		luaL_argcheck(L, client != NULL, 1, "invalid call. It should be MHttpClientLuaWrap!");

		const char* req = luaL_checkstring(L, 2);
		if (req != NULL)
		{
			(*client)->CreateRequest(req);
		}

		(*client)->Request();
		return 0;
	}

	static int autogc(lua_State* L)
	{
		MHttpClientLuaWrap** client = (MHttpClientLuaWrap**)luaL_checkudata(L, 1, "meta_http");
		luaL_argcheck(L, client != NULL, 1, "invalid call. It should be MHttpClientLuaWrap!");

		if (*client) delete (*client);

		return 0;
	}


	static const luaL_Reg httpnewlib[] = {
		{ "new", newclient },
		{ NULL, NULL }
	};

	static const luaL_Reg httpfunclib[] = {
		{ "register", reghttp },
		{ "request", request },
		{ "__gc", autogc },
		{ NULL, NULL }
	};

	int luaopen_http(lua_State* L) {
		luaL_newmetatable(L, "meta_http");		
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, httpfunclib, 0);

		luaL_newlib(L, httpnewlib);
		lua_setglobal(L, "Http");
		return 1;
	}
}