#pragma once
#include "MWindow.h"
#include "MDefine.h"
#include <vector>

struct SBmpInfo
{
	BITMAPINFO	m_BitmapInfo;
	RGBQUAD		m_bmiColors[2];	// 为BITMAPINFO的m_bmiColors补充两个元素空间
};

class MDrawProtocol;

class MLayer : public MWindow
{
public:
	MLayer(HINSTANCE instance, RECT rect, const char* name);
	virtual HRESULT Create();
	virtual void Show(int cmdShow);	
	
	void Render();

	void AddDrawInst(MDrawProtocol* draw);
	void RemoveDrawInst(MDrawProtocol* draw);
	
	void SetUpdateManual(bool manual);
protected:
	virtual void OnPaint(HDC hdc);
	virtual void OnDestroy();
	virtual void OnMainLoop();
	virtual void UpdateLayer();
	void ClearLayer();
protected:	
	HDC m_wndDc;
	HDC m_layerDc;
	UINT* m_backBuff;
	SBmpInfo* m_bmpInfo;
	HBITMAP m_bitMap;
	bool m_updateManual;

	std::vector<MDrawProtocol*> m_draws;
};

