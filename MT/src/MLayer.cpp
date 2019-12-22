#include "MLayer.h"
#include "MMath.h"
#include "bg.h"
#include "MTime.h"
#include "emt_hp.h"
#include "winfont.h"
#include "MSprite.h"
#include "pixelprocess.h"

MLayer::MLayer(HINSTANCE instance, RECT rect, const char* name)
	: MWindow(instance, rect, name)
	, m_layerDc(NULL)
	, m_wndDc(NULL)
	, m_backBuff(NULL)
	, m_bmpInfo(NULL)
	, m_bitMap(NULL) 
	, m_updateManual(true)
{
}

void MLayer::Show(int cmdShow)
{
	UpdateLayer();
}

HRESULT MLayer::Create()
{
	if (FAILED(Init()))
	{
		return -1;
	}

	DWORD style = WS_EX_LAYERED | WS_EX_TOOLWINDOW;
	m_hwnd = CreateWindowEx(style, m_name.c_str(), m_name.c_str(), WS_VISIBLE | WS_SYSMENU,
		m_rect.left, m_rect.top, GetWidth(), GetHeight(), NULL, NULL, m_instance, NULL);
	
	//ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);
	//ModifyMenu

	if (m_hwnd == NULL)
		return -1;

	UINT width = GetWidth();
	UINT height = GetHeight();

	m_backBuff = (UINT*)malloc(width * height * sizeof(UINT));
	memset(m_backBuff, 0, width * height * sizeof(UINT));

	m_wndDc = ::GetDC(m_hwnd);
	m_layerDc = ::CreateCompatibleDC(m_wndDc);
	m_bitMap = ::CreateCompatibleBitmap(m_wndDc, width, height);

	m_bmpInfo = (SBmpInfo*)malloc(sizeof(SBmpInfo));
	BITMAPINFO& bmpInfo = m_bmpInfo->m_BitmapInfo;
	bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
	bmpInfo.bmiHeader.biWidth = width;
	bmpInfo.bmiHeader.biHeight = -(int)height;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_BITFIELDS;

	*(UINT*)(bmpInfo.bmiColors + 0) = 0xFF0000;	// red����
	*(UINT*)(bmpInfo.bmiColors + 1) = 0x00FF00;	// green����
	*(UINT*)(bmpInfo.bmiColors + 2) = 0x0000FF;	// blue����

	::SelectObject(m_layerDc, m_bitMap);
	return S_OK;
}

#define RGBA(r,g,b,a)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16) | (((DWORD)(BYTE)(a))<<24)))

//#pragma comment(lib, "Msimg32.lib")  

void MLayer::UpdateLayer()
{
	UINT width = GetWidth();
	UINT height = GetHeight();

	::SetDIBitsToDevice(m_layerDc,
		0, 0, width, height,
		0, 0, 0, height,
		m_backBuff, &m_bmpInfo->m_BitmapInfo, DIB_RGB_COLORS);

	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = 1;
	blend.SourceConstantAlpha = 255;

	RECT rect;
	GetWindowRect(m_hwnd, &rect);

	POINT p = { rect.left, rect.top };
	SIZE s = { (LONG)width, (LONG)height };

	POINT m_originPos = { 0, 0 };
	::UpdateLayeredWindow(m_hwnd, m_wndDc, &p, &s, m_layerDc, &m_originPos, 0, &blend, ULW_ALPHA);
	
}

void MLayer::ClearLayer()
{
	UINT width = GetWidth();
	UINT height = GetHeight();
	memset(m_backBuff, 0, width * height * sizeof(UINT));
}

void MLayer::AddDrawInst(MDrawProtocol * draw)
{
	m_draws.push_back(draw);
}

void MLayer::RemoveDrawInst(MDrawProtocol * draw)
{
	std::vector<MDrawProtocol*>::iterator it = m_draws.begin();
	for (; it != m_draws.end(); ++it)
	{
		if (*it == draw)
		{
			m_draws.erase(it);
			break;
		}
	}
}

void MLayer::SetUpdateManual(bool manual)
{
	m_updateManual = manual;
}

void MLayer::OnPaint(HDC hdc)
{
	/*SetTextColor(m_layerDc, RGB(255, 0, 0));
	SetBkMode(m_layerDc, TRANSPARENT);
	TextOut(m_layerDc, 0, 10, TEXT("This is my window!"), (int)strlen(TEXT("This is my window!")));*/
}

void MLayer::OnDestroy()
{
	if (m_wndDc != NULL) ReleaseDC(m_hwnd, m_wndDc);

	if (m_backBuff != NULL) free(m_backBuff);
	if (m_bmpInfo != NULL) free(m_bmpInfo);
	if (m_bitMap != NULL) DeleteObject(m_bitMap);

	if (m_layerDc != NULL) ReleaseDC(m_hwnd, m_layerDc);

	MWindow::OnDestroy();
}

void MLayer::Render()
{
	ClearLayer();

	PixelSetContext(m_backBuff, GetWidth(), GetHeight());

	std::vector<MDrawProtocol*>::iterator it = m_draws.begin();
	for (; it != m_draws.end(); ++it)
	{
		(*it)->Draw();
	}

	UpdateLayer();
}

void MLayer::OnMainLoop()
{
	//update
	if (!m_updateManual) return;	

	Render();
}
