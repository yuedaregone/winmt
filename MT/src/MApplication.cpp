#include "MApplication.h"
#include "MWindow.h"
#include "resource.h"
#include "MTouchControl.h"
#include <windowsx.h>
#include "MTime.h"
#include "MKeyboardListen.h"
#include "MLuaEngine.h"
#include "MTimerManager.h"
#include <assert.h>

MApplaction* MApplaction::m_current = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MApplaction* app = MApplaction::GetCurrent();
	if (app == NULL || !app->AppWndProc(hWnd, message, wParam, lParam))
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MApplaction* app = MApplaction::GetCurrent();
	assert(app != NULL);

	HHOOK hook = app->GetHHOOK();

	if (nCode != HC_ACTION) 
		return CallNextHookEx(hook, nCode, wParam, lParam);

	
	if (nCode >= 0)
	{
		if (app->AppKeyboardProc(nCode, wParam, lParam))
		{
			return 1;
		}		
	}
	return CallNextHookEx(app->GetHHOOK(), nCode, wParam, lParam);
}

MApplaction::MApplaction(HINSTANCE hinstance)
	: m_instance(hinstance)
	, m_proc(WndProc)
	, m_waitMsg(true)
	, m_keyHook(NULL)
{
	m_current = this; 
	m_interval.QuadPart = 0;
}

void MApplaction::Add(MWindow * window)
{
	MWindow* mw = Get(window->GetName());
	if (mw == nullptr)
	{
		window->SetWndProc(m_proc);
		m_windows.push_back(window);
	}
}

MWindow * MApplaction::Get(const std::string& name)
{
	std::vector<MWindow*>::iterator it = m_windows.begin();
	for (; it != m_windows.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			return *it;
		}
	}
	return nullptr;
}

void MApplaction::Destroy(const std::string& name)
{
	std::vector<MWindow*>::iterator it = m_windows.begin();
	for (; it != m_windows.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			(*it)->End();
			break;
		}
	}
}

bool MApplaction::AppWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	switch (message)
	{
		case WM_LBUTTONDOWN:			
			m_capture = g_mTouchControl->OnTouchBegin(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			if (m_capture)
				SetCapture(hWnd);
			else
				::SendMessageA(hWnd, WM_SYSCOMMAND, 0xF012, 0);	
			return true;
		case WM_MOUSEMOVE:
			g_mTouchControl->OnTouchMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return true;
		case WM_LBUTTONUP:
			if (m_capture)
			{
				g_mTouchControl->OnTouchEnd(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				ReleaseCapture();
			}	
			m_capture = false;
			return true;
		case WM_PAINT:
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hWnd, &ps);
			OnPaint(hdc);
			EndPaint(hWnd, &ps);
			return true;
		case WM_CLOSE:
			::DestroyWindow(hWnd);
			return true;
		case WM_DESTROY:
			OnDestroyWindow(hWnd);
			return true;		
	}
	return false;
}

bool MApplaction::AppKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const char *info = NULL;
	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
	if (nCode >= 0)
	{
		bool ret = false;
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			ret = g_mKeyboardListen->OnKeyEvent(KEY_DOWN, p->vkCode);
		}
		else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
		{
			ret = g_mKeyboardListen->OnKeyEvent(KEY_UP, p->vkCode);
		}
		return ret;
	}
	return false;
}

void MApplaction::LoopWaitMessage()
{
	HACCEL hAccelTable = LoadAccelerators(m_instance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	MSG msg = { 0 };

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		this->OnFrameEnd();
	}
}

void MApplaction::LoopPeekMessage()
{
	LARGE_INTEGER freq;

	LARGE_INTEGER now;
	LARGE_INTEGER last;
	LONGLONG interval;
	
	QueryPerformanceFrequency(&freq);
	MTime::SetFrequency(freq.QuadPart);
	
	QueryPerformanceCounter(&last);

	MSG msg = { 0 };

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			QueryPerformanceCounter(&now);
			interval = now.QuadPart - last.QuadPart;
			if (interval > m_interval.QuadPart)
			{
				MTime::UpdateTime(interval);

				last.QuadPart = now.QuadPart;

				g_mTimerManager->Update();
				g_mLuaEngine->OnMainLoop();
				MainLoop();
			}
			else
			{
				Sleep(1);
			}
			this->OnFrameEnd();
		}
	}
}

void MApplaction::Run()
{
	if (m_waitMsg)
	{
		LoopWaitMessage();
	}
	else
	{
		LoopPeekMessage();
	}

	OnDestroyApplication();
}

void MApplaction::MainLoop()
{
	std::vector<MWindow*>::iterator it = m_windows.begin();
	for (; it != m_windows.end(); ++it)
	{
		(*it)->OnMainLoop();
	}
}

void MApplaction::SetInterval(float interval)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	m_interval.QuadPart = (LONGLONG)(interval * freq.QuadPart);
}

void MApplaction::OnFrameEnd()
{
	std::vector<MWindow*>::iterator it = m_windows.begin();
	for (; it != m_windows.end();)
	{
		if ((*it)->IsEnd())
		{
			(*it)->ReleaseWindow();
			delete (*it);
			it = m_windows.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void MApplaction::OnDestroyWindow(HWND hwnd)
{
	std::vector<MWindow*>::iterator it = m_windows.begin();
	for (; it != m_windows.end(); ++it)
	{
		if ((*it)->GetHwnd() == hwnd)
		{
			(*it)->OnDestroy();
			break;
		}
	}
}

void MApplaction::OnPaint(HDC hdc)
{
	std::vector<MWindow*>::iterator it = m_windows.begin();
	for (; it != m_windows.end(); ++it)
	{
		(*it)->OnPaint(hdc);		
	}
}

void MApplaction::OnDestroyApplication()
{
	if (m_keyHook != NULL)
	{
		StopHOOK();
	}
}

void MApplaction::StartHOOK()
{
	m_keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, m_instance, 0);

	if (m_keyHook == NULL)
	{
		MessageBox(NULL, "Create Hook Error!", "HOOK", MB_OK);
	}
}

void MApplaction::StopHOOK()
{
	if (m_keyHook != NULL)
	{
		UnhookWindowsHookEx(m_keyHook);
		m_keyHook = NULL;
	}	
}
