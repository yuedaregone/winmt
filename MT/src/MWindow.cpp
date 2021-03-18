#include "MWindow.h"
#include "resource.h"
#include "MMath.h"
 
MWindow::MWindow(HINSTANCE instance, RECT rect, const char* name)
	: m_instance(instance)
	, m_rect(rect)
	, m_hwnd(NULL)
	, m_name(name)
	, m_wndProc(NULL)	
	, m_end(false)
{
}

MWindow::~MWindow()
{	
}

void MWindow::SetWndProc(WNDPROC proc)
{
	m_wndProc = proc;
	SetWindowLong(m_hwnd, GWLP_WNDPROC, (LONG)m_wndProc); 
}

void MWindow::Show(int cmdShow)
{
	::ShowWindow(m_hwnd, cmdShow);
	UpdateWindow(m_hwnd);		
}

void MWindow::End()
{
	m_end = true;
	SendMessage(m_hwnd, WM_CLOSE, 0, 0);
}

std::string MWindow::GetName()
{
	return m_name;
}

HRESULT MWindow::Init()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = m_wndProc == NULL ? DefWindowProc : m_wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_instance;
	wcex.hIcon = LoadIcon(m_instance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = m_name.c_str();
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	if (!RegisterClassEx(&wcex))
	{
		printf("RegisterClassEx %s Error: %d\n", m_name.c_str(), GetLastError());
		return -1;
	}		
	return S_OK;
}

HRESULT MWindow::Create()
{
	if (FAILED(Init()))
	{
		return -1;
	}

	DWORD style = WS_EX_TOPMOST | WS_EX_ACCEPTFILES;
	m_hwnd = CreateWindowEx(style, m_name.c_str(), m_name.c_str(),
		WS_VISIBLE | WS_SYSMENU,
		m_rect.left, m_rect.top, GetWidth(), GetHeight(),
		NULL, NULL, m_instance, NULL);

	if (!m_hwnd)
	{
		printf("CreateWindowEx %s Error: %d\n", m_name.c_str(), GetLastError());
		return -1;
	}	
	return S_OK;
}

int MWindow::LoopMessage()
{
	HACCEL hAccelTable = LoadAccelerators(m_instance, MAKEINTRESOURCE(IDR_ACCELERATOR1));
	MSG msg = { 0 };

	/*while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}*/	
		
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//TODO: Update And Render


		}
	}
	return (int)msg.wParam;
}

void MWindow::OnPaint(HDC hdc)
{
	//SetTextColor(hdc, RGB(255, 0, 0));
	//SetBkMode(hdc, TRANSPARENT);
	//TextOut(hdc, 0, 0, TEXT("This is my window!"), strlen(TEXT("This is my window!")));
}

void MWindow::OnDestroy()
{
	m_end = true;
}

void MWindow::OnMainLoop()
{
}

void MWindow::ReleaseWindow()
{
	if (!UnregisterClass(m_name.c_str(), m_instance))	
	{
		printf("UnregisterClass %s Error: %d\n", m_name.c_str(), GetLastError());
	}
}
