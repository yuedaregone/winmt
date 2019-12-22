#pragma once
#include "MMath.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
	typedef enum
	{
		eOverride,
		eAlphaBlend,
		eAlphaAdditive,
	}PixelType;
		
	void PixelSetContext(uint32_t* buf, uint32_t wWidth, uint32_t wHeight);
	void PixelSetBlitFunc(PixelType type);
	void PixelSetScale(bool enabal, float scaleX, float scaleY);
	void PixelSetRotation(bool enabal, float rot);

	void BlitContent(uint32_t * srcData, uint32_t srcWidth, uint32_t srcHeight, uint32_t x, uint32_t y);	
	void BlitGrayToColor(uint8_t* srcData, uint32_t srcWidth, uint32_t srcHeight, uint32_t x, uint32_t y, uint32_t colorRGBA);
#ifdef __cplusplus
	}
#endif
