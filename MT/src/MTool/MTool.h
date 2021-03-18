#pragma once
#include <windows.h> 
#include <string>

extern "C" {

	RECT GetRect(LONG x, LONG y, LONG w, LONG h);
	RECT GetDesktopRect();
	RECT GetCenterRect(LONG w, LONG h);
	RECT GetLeftTopRect(LONG w, LONG h);
	RECT GetLeftBottomRect(LONG w, LONG h);
	RECT GetRightTopRect(LONG w, LONG h);
	RECT GetRightBottomRect(LONG w, LONG h);

	uint32_t ConvertColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

	bool IsFileExit(const char* filename);

	int unicode_len(const char chr);
	int get_unicode(const char *str, int n);
}

std::string GetCurrentPath();

std::string GetFullPath(const char* str);
