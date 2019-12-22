#pragma once
#include <windows.h>
#include <vector>

class MWindow;

class MApplaction
{
public:
	MApplaction(HINSTANCE hinstance);
	static MApplaction* GetCurrent() { return m_current; }

public: //Window	
	void Add(MWindow* window);
	MWindow* Get(const std::string& name);
	void Destroy(const std::string& name);
	bool AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void Run();
	void SetWaitMessageMode(bool wait) { m_waitMsg = wait; }
	void MainLoop();
	void SetInterval(float interval);
	HINSTANCE GetHInstance() { return m_instance; }
private:
	void LoopPeekMessage();
	void LoopWaitMessage();

	void OnPaint(HDC hdc);
	void OnDestroyApplication();
	void OnDestroyWindow(HWND hwnd);
	void OnFrameEnd();
public: //Hook
	bool AppKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
	HHOOK GetHHOOK() { return m_keyHook; }
	void StartHOOK();
	void StopHOOK();	
private:
	static MApplaction* m_current;
	std::vector<MWindow*> m_windows;
	HINSTANCE m_instance;
	WNDPROC m_proc;
	bool m_capture;
	HHOOK m_keyHook;
	bool m_waitMsg;
	LARGE_INTEGER m_interval;
};