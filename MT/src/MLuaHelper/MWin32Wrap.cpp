#include "MLuaWrap.h"
#include "MLuaEngine.h"
#include <windows.h>

extern "C" {
	static int screencount(lua_State* L) {
		int dspNum = ::GetSystemMetrics(SM_CMONITORS);
		lua_pushinteger(L, dspNum);		
		return 1;
	}

	typedef struct tagScreenInfo {
		int index;
		RECT rect[4];
	}ScreenInfo;

	static BOOL CALLBACK MonitorEnum(HMONITOR hMon, HDC hdc, LPRECT lprcMonitor, LPARAM pData)
	{
		ScreenInfo* screenInfo = (ScreenInfo*)pData;
		screenInfo->rect[screenInfo->index] = *lprcMonitor;
		screenInfo->index++;
		return TRUE;
	}

	static int creenrect(lua_State* L) {
		int index = (int)luaL_checkinteger(L, 1);
		int dspNum = ::GetSystemMetrics(SM_CMONITORS);
		luaL_argcheck(L, index >= 0 && index < dspNum, 1, "invalid creen index!");
		
		ScreenInfo screenInfo;
		screenInfo.index = 0;
		EnumDisplayMonitors(0, 0, MonitorEnum, (LPARAM)&screenInfo);

		RECT rect = screenInfo.rect[index];
		LONG left = rect.left;
		LONG right = rect.right;
		LONG top = rect.top;
		LONG bottom = rect.bottom;

		lua_pushinteger(L, left);
		lua_pushinteger(L, right);
		lua_pushinteger(L, top);
		lua_pushinteger(L, bottom);

		return 4;
	}

	static const luaL_Reg win32lib[] = {
		{"screencount", screencount },
		{"creenrect", creenrect},
		{NULL, NULL}
	};

	int luaopen_win(lua_State* L) {
		luaL_newlib(L, win32lib);
		lua_setglobal(L, "win");
		return 1;
	}
}