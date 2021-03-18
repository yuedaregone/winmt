#pragma once
#include "MTool.h"

extern "C" {
	uint32_t ConvertColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		uint32_t color = (uint32_t)a & 0x000000FF;		
		color |= (uint32_t)b << 8 & 0x0000FF00;
		color |= (uint32_t)g << 16 & 0x00FF0000;
		color |= (uint32_t)r << 24 & 0xFF000000;

		return color;
	}
}