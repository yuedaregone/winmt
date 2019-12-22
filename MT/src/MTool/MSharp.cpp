#pragma once
#include "MTool.h"

extern "C" {
	RECT GetRect(LONG x, LONG y, LONG w, LONG h)
	{
		RECT rt;
		rt.left = x;
		rt.right = x + w;
		rt.top = y;
		rt.bottom = y + h;
		return rt;
	}

	RECT GetDesktopRect()
	{
		RECT rect;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		return rect;
	}

	RECT GetCenterRect(LONG w, LONG h)
	{
		RECT desktop = GetDesktopRect();
		LONG centerX = desktop.right / 2;
		LONG centerY = desktop.bottom / 2;
		return GetRect(centerX - w / 2, centerY - h / 2, w, h);
	}

	RECT GetLeftTopRect(LONG w, LONG h)
	{
		return GetRect(0, 0, w, h);
	}	

	RECT GetLeftBottomRect(LONG w, LONG h)
	{
		RECT desktop = GetDesktopRect();
		return GetRect(0, desktop.bottom - h, w, h);
	}

	RECT GetRightTopRect(LONG w, LONG h)
	{
		RECT desktop = GetDesktopRect();
		return GetRect(desktop.right - w, 0, w, h);
	}	

	RECT GetRightBottomRect(LONG w, LONG h)
	{
		RECT desktop = GetDesktopRect();
		return GetRect(desktop.right - w, desktop.bottom - h, w, h);
	}
}
