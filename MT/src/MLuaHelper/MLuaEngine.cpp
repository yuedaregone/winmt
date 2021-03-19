#include "MLuaEngine.h"
#include "MLuaWrap.h"
#include "MTool.h"
#include <string>

static const char* search_list[] = {
	"./lua/",
	"../lua/",
	NULL
};

static std::string GetSearchPath(const char* filename)
{
	if (IsFileExit(filename)) 
	{
		return filename;
	}

	std::string path;

	int i = 0;
	const char* search = NULL;
	while ((search = search_list[i++]) != NULL)
	{
		path.clear();
		path.append(search);
		path.append(filename);

		if (IsFileExit(path.c_str())) {
			return path;
		}
	}

	path.clear();
	return path;
}

MLuaEngine::MLuaEngine()
	: m_L(NULL)
{
}

void MLuaEngine::Init()
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);	
	m_L = L;

	AddSearchPath("path", "./lua/?.lua;../lua/?.lua;");
	//AddSearchPath("cpath", "");

	luaopen_mall(L);

	if (TryStartMainLua())
	{
		ExecuteGlobalFunction("Start");

		ExecuteCacheFunction("Tick");
	}
}

void MLuaEngine::OnMainLoop()
{
	ExecuteCacheFunction("Tick");
}

bool MLuaEngine::TryStartMainLua()
{
	std::string mainLua = GetSearchPath("main.lua");
	
	if (luaL_loadfile(m_L, mainLua.c_str()) || lua_pcall(m_L, 0, 0, 0))
	{
		printf("LUA ERROR:%s", lua_tostring(m_L, -1));
		lua_close(m_L);
		m_L = NULL;
		return false;
	}

	return true;
}

void MLuaEngine::OnDestroy()
{
	ExecuteGlobalFunction("End");
}

void MLuaEngine::AddSearchPath(const char* name, const char* value)
{
	lua_getglobal(m_L, "package");
	lua_getfield(m_L, -1, name);

	std::string v;
	v.append(lua_tostring(m_L, -1));
	v.append(";");
	v.append(value);
	lua_pushstring(m_L, v.c_str());
	lua_setfield(m_L, -3, name);
	lua_pop(m_L, 2);
}

void MLuaEngine::ExecuteTableFunction(const char * tabName, const char * funName)
{
	if (m_L != NULL && lua_getglobal(m_L, tabName) == LUA_TTABLE)
	{
		lua_pushstring(m_L, funName);
		lua_gettable(m_L, -2);
		lua_pcall(m_L, 0, 0, 0);
	}
}

void MLuaEngine::ExecuteGlobalFunction(const char * funName)
{
	if (m_L != NULL && lua_getglobal(m_L, funName) == LUA_TFUNCTION)
	{
		lua_pcall(m_L, 0, 0, 0);
	}
}

void MLuaEngine::ExecuteGlobalFunction(const char* funName, int argc, ...)
{
	if (m_L != NULL && lua_getglobal(m_L, funName) == LUA_TFUNCTION)
	{
		va_list args;
		va_start(args, argc);
		for (int i = 0; i < argc; ++i)
		{
			const char* arg = va_arg(args, char *);
			lua_pushstring(m_L, arg);
		}		
		va_end(args);

		lua_pcall(m_L, argc, 0, 0);
	}
}

void MLuaEngine::ExecuteCacheFunction(const char * funName)
{
	if (m_L == NULL) return;

	int lua_function = LUA_REFNIL;
	if (m_mapFunc.find(funName) != m_mapFunc.end())
	{
		lua_function = m_mapFunc[funName];		
	}
	else
	{
		if (lua_getglobal(m_L, funName) == LUA_TFUNCTION)
		{
			lua_function = luaL_ref(m_L, LUA_REGISTRYINDEX);
			m_mapFunc[funName] = lua_function;
		}
	}	

	if (lua_function != LUA_REFNIL)
	{
		lua_rawgeti(m_L, LUA_REGISTRYINDEX, lua_function);
		lua_pcall(m_L, 0, 0, 0);
	}
}

void MLuaEngine::ReleaseCacheFunction(int fun)
{
	if (fun != LUA_REFNIL)
	{
		lua_State* L = g_mLuaEngine->GetL();
		luaL_unref(L, LUA_REGISTRYINDEX, fun);
	}
}
