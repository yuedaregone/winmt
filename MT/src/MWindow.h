#pragma once
#include <windows.h>
#include <string>

class MApplaction;

class MWindow
{
public:
	MWindow(HINSTANCE instance, RECT rect, const char* name);	
	~MWindow();
	void SetWndProc(WNDPROC proc);
	virtual void Show(int cmdShow);
	void End();
	bool IsEnd() { return m_end; };
	std::string GetName();
	const char* GetNameCStr() { return m_name.c_str(); }
	int LoopMessage();
	virtual HRESULT Create();

	inline UINT GetWidth() { return m_rect.right - m_rect.left; }
	inline UINT GetHeight() { return m_rect.bottom - m_rect.top; }
	inline HWND GetHwnd() { return m_hwnd; }

protected:
	virtual void OnPaint(HDC hdc);
	virtual void OnDestroy(); //release self data	
	virtual void OnMainLoop();
	void ReleaseWindow();
	HRESULT Init();		
protected:
	HINSTANCE m_instance;
	HWND m_hwnd;
	RECT m_rect;
	std::string m_name;
	WNDPROC m_wndProc;
	bool m_end;

	friend MApplaction;
};

