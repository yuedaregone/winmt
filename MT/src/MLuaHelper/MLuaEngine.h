#pragma once
#include "MSingleton.h"
#include "lua.hpp"
#include <map>
 
class MApplaction;

class MLuaEngine : public MSingleton<MLuaEngine>
{
public:
	MLuaEngine();
	void Init();

	lua_State* GetL() { return m_L; }
private:
	void OnMainLoop();
	bool TryStartMainLua();
	void OnDestroy();

public:
	void ExecuteTableFunction(const char* tabName, const char* funName);
	void ExecuteGlobalFunction(const char* funName);
	void ExecuteGlobalFunction(const char* funName, int argc, ...);	

	void ExecuteCacheFunction(const char* funName);

	void ReleaseCacheFunction(int fun);
private:
	lua_State* m_L;
	std::map<const char*, int> m_mapFunc;

	friend MApplaction;
};

#define g_mLuaEngine MLuaEngine::getInstance()
